# Dokümantasyon: `RS422_485GuiApp` Projesi için Linux AppImage Dağıtımı

**Proje:** RS422_485GuiApp  
**Hedef Platform:** Linux (x86_64)  
**Çıktı Formatı:** AppImage  
**Sürüm:** 1.0  
**Tarih:** 24.05.2024

---

## 1. Giriş

### 1.1. Amaç
Bu dokümanın amacı, `RS422_485GuiApp` projesinin, Qt kurulu olmayan herhangi bir modern Linux bilgisayarında çalışacak, tek bir taşınabilir **AppImage** dosyası olarak paketlenme sürecini detaylandırmaktır. Süreç, Qt Creator üzerinden otomatikleştirilecektir.

### 1.2. Nihai Hedef
Süreç sonunda, `build` dizini altında `RS422_485GuiApp-1.0.0-x86_64.AppImage` gibi bir dosya elde edilecektir. Bu dosya, başka bir Linux bilgisayara kopyalanıp çalıştırma izni verildikten sonra doğrudan çalıştırılabilir olacaktır.

### 1.3. Kullanılan Ana Araçlar
*   **Qt Creator:** Geliştirme ve derleme arayüzü.
*   **CMake:** Proje derleme sistemi.
*   **`linuxdeployqt`:** Qt uygulamalarını ve bağımlılıklarını paketleyen ana araç.

---

## 2. Ortam Hazırlığı

Paketleme işlemine başlamadan önce, derlemenin yapılacağı Linux makinesinde bazı hazırlıkların yapılması gerekmektedir.

### 2.1. `linuxdeployqt` Aracının İndirilmesi
`linuxdeployqt` aracı, resmi paket depolarında bulunmaz. Manuel olarak indirilmelidir.

1.  **İndirme:** Araç, geliştiricisinin GitHub sayfasından indirilir. Merkezi bir yerde (`~/Tools`) saklanması tavsiye edilir.
    ```bash
    mkdir -p ~/Tools && cd ~/Tools
    wget -c "https://github.com/probonopd/linuxdeployqt/releases/download/continuous/linuxdeployqt-continuous-x86_64.AppImage"
    ```

2.  **Çalıştırma İzni Verme:** İndirilen dosyaya çalıştırma izni verilir.
    ```bash
    chmod a+x linuxdeployqt-continuous-x86_64.AppImage
    ```
    Bu adımdan sonra araç, tam yolu (`/home/trick/Tools/linuxdeployqt-continuous-x86_64.AppImage`) belirtilerek kullanılabilir.

### 2.2. Gerekli Sistem Paketleri
`linuxdeployqt`'nin çalışabilmesi için `patchelf` paketine ihtiyacı vardır.
```bash
sudo apt-get install patchelf
```

---

## 3. Projenin Dağıtıma Hazırlanması (Kaynak Kodu Ayarları)

`linuxdeployqt` aracının hatasız çalışması için proje dosyalarında bazı kritik düzenlemeler yapılmıştır.

### 3.1. Proje Adının Düzeltilmesi (Slash Karakteri Sorunu)
*   **Sorun:** Proje adında (`RS422/485GuiApp`) bulunan `/` karakteri, Linux dosya sisteminde dizin ayıracı olduğu için AppImage oluşturulurken "Could not create destination file" hatasına neden oluyordu.
*   **Çözüm:** Projenin ana `CMakeLists.txt` dosyasında, `project()` ve `add_executable()` komutlarındaki isim, `/` karakteri içermeyecek şekilde `RS422_485GuiApp` olarak güncellendi.

### 3.2. `.desktop` Dosyasının Eklenmesi (`Categories` Sorunu)
*   **Sorun:** `appimagetool` (linuxdeployqt'nin bir parçası), uygulamanın menülerde görünmesi için bir `.desktop` dosyası ve bu dosyanın içinde `Categories=` anahtarı olmasını zorunlu kılıyordu. Bu eksiklik, "Categories entry not found" hatasına yol açıyordu.
*   **Çözüm:** Projenin kaynak kod dizinine (`RS422-485GuiApp/`) aşağıdaki içerikle `RS422_485GuiApp.desktop` adında bir dosya eklendi:
    ```ini
    [Desktop Entry]
    Type=Application
    Name=RS422/485 GuiApp
    Comment=A tool for RS422/485 communication
    Exec=RS422_485GuiApp
    Icon=app_icon
    Categories=Utility;Development;
    ```

### 3.3. İkon Dosyasının Eklenmesi (`Icon` Sorunu)
*   **Sorun:** `.desktop` dosyasında `Icon=app_icon` satırı belirtildiği için, `linuxdeployqt` bu isimde bir ikon dosyası (`app_icon.png`) bulmayı bekliyordu. Bulamadığında "Icon file missing" hatası veriyordu.
*   **Çözüm:** Projenin kaynak kod dizinine (`RS422-485GuiApp/`) `app_icon.png` adında geçerli bir PNG dosyası eklendi.

### 3.4. CMake Entegrasyonu (`file(COPY)`)
*   **Sorun:** Oluşturulan `.desktop` ve `.png` dosyalarının, `linuxdeployqt`'nin onları bulabilmesi için derleme dizinine kopyalanması gerekiyordu.
*   **Çözüm:** Projenin `CMakeLists.txt` dosyasına aşağıdaki komut eklenerek bu kopyalama işlemi otomatikleştirildi:
    ```cmake
    # .desktop ve ikon dosyalarını, derleme dizinine kopyala.
    file(COPY 
        RS422_485GuiApp.desktop 
        app_icon.png 
        DESTINATION ${CMAKE_BINARY_DIR}
    )
    ```

---

## 4. Qt Creator ile Otomatik Paketleme Süreci

Tüm hazırlıklar tamamlandıktan sonra, paketleme süreci Qt Creator'ın derleme adımlarına tek bir "Custom Process Step" eklenerek entegre edildi.

1.  **"Release" Moduna Geçiş:** Proje, sol alttaki menüden **"Release"** moduna ayarlandı.
2.  **Özel Derleme Adımı Yapılandırması:** `Projects > Build > Build Steps > Add Build Step > Custom Process Step` yolu izlenerek aşağıdaki ayarlar yapıldı:

    *   **Command:** `bash`
    *   **Arguments:**
        ```bash
        -c 'export VERSION=1.0.0 && export LD_LIBRARY_PATH=%{Qt:QT_INSTALL_LIBS} && export PATH=%{Qt:QT_INSTALL_BINS}:$PATH && /home/trick/Tools/linuxdeployqt-continuous-x86_64.AppImage "%{buildDir}/%{CurrentProject:Name}" -appimage'
        ```

### 4.1. Argümanların Detaylı Açıklaması
*   `export VERSION=1.0.0`: AppImage için bir versiyon numarası belirler.
*   `export LD_LIBRARY_PATH=%{Qt:QT_INSTALL_LIBS}`: **En Kritik Adım.** Qt kütüphanelerinin bulunduğu yolu sisteme bildirir. Bu, `libQt6Gui.so.6 => not found` hatasını çözer.
*   `export PATH=%{Qt:QT_INSTALL_BINS}:$PATH`: Qt'nin `qmake` gibi araçlarını `PATH`'e ekler.
*   `/home/trick/Tools/...`: `linuxdeployqt` aracının tam yolu.
*   `"%{buildDir}/%{CurrentProject:Name}"`: Derlenen çalıştırılabilir dosyanın yolu.
*   `-appimage`: Sonucun tek bir AppImage dosyası olarak paketlenmesini sağlar.

---

## 5. Karşılaşılan Hatalar ve Uygulanan Çözümler

Bu süreçte karşılaşılan tüm hatalar ve çözümleri aşağıda özetlenmiştir.

| Hata Mesajı | Sebep | Çözüm |
| :--- | :--- | :--- |
| `E: Unable to locate package linuxdeployqt` | Araç, `apt` ile kurulamaz. | 3.1. adımda anlatıldığı gibi manuel olarak indirildi. |
| `The program ".../linuxdeployqt..." does not exist` | Qt Creator'a girilen yol yanlıştı (`/home/Tools` yerine `/home/trick/Tools` olmalıydı). | Yol düzeltildi ve `chmod +x` ile çalıştırma izni verildi. |
| `libQt6Gui.so.6 => not found` | `ldd` aracı, standart dışı yoldaki Qt kütüphanelerini bulamıyordu. | 4.1. adımda anlatıldığı gibi komuta `export LD_LIBRARY_PATH` eklendi. |
| `.desktop file is missing a Categories= key` | `appimagetool`, `Categories=` anahtarını zorunlu kılıyordu. | 3.2. adımda anlatıldığı gibi `Categories=` satırı içeren bir `.desktop` dosyası oluşturuldu. |
| `Icon file missing` | `.desktop` dosyasında belirtilen ikon dosyası bulunamıyordu. | 3.3. adımda anlatıldığı gibi projeye bir `app_icon.png` dosyası eklendi. |
| `Could not create destination file` | Proje adındaki `/` karakteri geçersiz dosya adına neden oluyordu. | 3.1. adımda anlatıldığı gibi proje adı `RS422_485GuiApp` olarak değiştirildi. |
| `The command "..." finished successfully` ama en sonda `Error while building...` | Paketleme başarılı olmuş, ancak Qt Creator'ın son "Run" adımı eski yapılandırma nedeniyle başarısız olmuştu. | Bu bir başarı göstergesidir. İsteğe bağlı olarak `Projects > Run` ayarlarından çalıştırılacak dosya AppImage olarak değiştirilebilir. |