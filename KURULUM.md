# MacroSlots - Kurulum Rehberi (adim adim)

Bu mod Geometry Dash icin yapilmis bir **replay botudur**. Makro (.gdr2) dosyalarini
yukler, **1-9** tuslariyla aninda oynatir, **0** ile kapatir.

> Not: Bu mod sadece makro **oynatir**. Kendi kaydini yapmak istersen ayrica
> Eclipse Menu veya xdBot gibi bir mod lazimdir.

---

## 0) Oncelikle bunlari kontrol et

1. Geometry Dash'in **Steam'den orijinal ve guncel** surumunde oldugundan emin ol.
2. Oyunda **Geode** kurulu olmali (ana menu'de "Geode" butonu gorunuyorsa kuruludur).
   Degilse: <https://geode-sdk.org/install> -> Windows -> indir -> calistir -> oyunu ac.
3. Oyunu kapat (derleme yaparken acik olmasinda sakince yok ama kapali olmasi daha temiz).

---

## 1) Gelistirici araclarini kur (bir kez yapilir)

Hepsi ucretsizdir. Sirayla kur:

### a) Visual Studio Build Tools 2022

1. Buradan indir: <https://aka.ms/vs/17/release/vs_BuildTools.exe>
2. Calistir. Kurucuda **"Calisma Surumu (Workloads)"** ekraninda
   **"Masaustu gelistirmesi ile C++" (Desktop development with C++)** kutusunu isaretle.
3. Sag alttaki **"Degistir" (Install)** butonuyla kur. (5-15 dk surebilir)

### b) CMake

1. Buradan indir: <https://cmake.org/download/> -> "Windows x64 Installer" (`.msi`)
2. Kurarken **"Add CMake to the system PATH"** secenegini isaretlemeyi unutma.

### c) Geode CLI (komut araci)

En kolay yolu PowerShell penceresine su komutu yapistirmaktir:

```powershell
winget install GeodeSDK.GeodeCLI
```

`winget` yoksa ya da hata verirse:
<https://github.com/geode-sdk/cli/releases> adresinden en yeni `geode-Windows.zip`
dosyasini indir, icerigini `C:\geode-cli` klasorune ac, sonra bu klasoru PATH'e ekle:

1. Windows arama cubuguna "ortam degiskenleri" yaz -> **"Sistemin icin ortam degiskenlerini duzenle"**
2. ustteki **"Path"** satirini sec -> **Duzenle** -> **Ekle** -> `C:\geode-cli` -> Tamam

### d) Geode SDK'yi indir

Yeni bir **PowerShell** penceresi ac ve su komutu calistir:

```powershell
geode sdk install
```

Bu komut oyunun surumune uygun Geode gelistirme dosyalarini indirir (bir kez yeter).

> Kontrol etmek icin: `geode --version` yaz, surum yaziyorsa her sey hazir.

---

## 2) Modu derle

1. `MacroSlots` klasorune git
   (`C:\Users\test\Documents\Varsayilan Proje\MacroSlots`)
2. Klasorun icindeyken **Shift + Sag tik** yap -> **"PowerShell penceresinde ac"**
3. Su komutu ver:

```powershell
geode build
```

4. Ilk seferde birkac dakika surer (internetten bazi seyler indirilir).
   Sonda su benzeri bir cikti gorursun:

```
[INFO] Packaged mod at .../build/MacroSlots.geode
```

**Tebrikler, modun hazir!** `build\MacroSlots.geode` dosyasi olustu.

---

## 3) Modu oyuna kur

Iki yoldan biri:

**Yol A (kolay):** Oyunu ac -> Geode menusu -> **Mods** sekmesi -> **"Install from file"**
-> `build\MacroSlots.geode` dosyasini sec -> oyunu yeniden baslat.

**Yol B:** `MacroSlots.geode` dosyasini su klasore kopyala:

```
%localappdata%\GeometryDash\geode\mods\
```

Sonra oyunu ac.

Geode menusunde **MacroSlots** modunu **acik (enabled)** goruntule.

---

## 4) Makro dosyalarini yerlestir

Makro klasoru (ilk acilista otomatik olusur):

```
%localappdata%\GeometryDash\geode\mods\kanka.macroslo\macros\
```

Bu klasore `.gdr2` (veya `.gdr`) makro dosyalarini kopyala.
Klasore oyun icinden de ulasabilirsin: Sag Shift -> **Klasoru Ac** butonu.

---

## 5) Kullanim

| Tus | Ne yapar |
| --- | --- |
| **Sag Shift** | Menuyu acar / kapatir |
| **+** (menude) | O slota makro atar |
| **Apply** | Atamalari kaydeder |
| **1 - 9** | O slottaki makroyu oynatmaya baslar (ekranda yazi cikmaz) |
| **0** | Botu kapatir, oyun normal moda doner |

Ornek kullanis:
1. Sag Shift ile menuyu ac
2. 1 numaranin altindaki **+**'a tikla -> "heliopolis.gdr2" sec
3. 2 numaranin altindaki **+**'a tikla -> "bloodbath.gdr2" sec
4. **Apply**'e bas -> kaydedildi (bildirim gorunur)
5. Menuyu kapat (Sag Shift), leveli ac
6. **1** -> heliopolis oynar. Olunce/tekrar deneyince otomatik basa doner.
7. **2** -> ayni levelde aninda bloodbath macro'suna gecis
8. **0** -> bot kapali, sen oynuyorsun

---

## Sorun giderme

**"geode komutu taninmiyor"**
CLI PATH'e eklenmemis demektir. 1-c adimini tekrar uygula ya da PowerShell'de
`C:\geode-cli\geode.exe --version` seklinde tam yolla dene.

**"Unable to find Geode SDK! Please define GEODE_SDK..."**
```powershell
geode sdk install
```
komutunu calistir. Hala olmuyorsa Geode'u elle tanimlayabilirsin:
```powershell
[Environment]::SetEnvironmentVariable("GEODE_SDK", "C:\Users\test\AppData\Local\Temp\opencode\geode", "User")
```
(not: bu ornek bir yoldur; asil Geode klasorunun yolunu `geode sdk install`
ciktisinda goreceksin.)

**Derleme "geode" surumuyle ilgili hata verirse (mod.json)**
PowerShell'de `geode --version` yaz, cikan surumu (ornegin `5.0.3`) al ve
`mod.json` dosyasindaki `"geode": "v5.0.0"` yazan yeri kendi surumunle degistir
(`"geode": "v5.0.3"`). Sonra tekrar `geode build`.

**Oyunda "wrong game version" / mod yuklenmiyor**
`mod.json` icindeki `"gd": { "win": "2.2091" }` kismi oyunun surumuyle ayni olmali.
Geode menusunde oyunun surumu yazar (orn. 2.2081, 2.2091). Oraya gore duzelt,
`geode build` ile tekrar derle ve kur.

**Derleme "processCommands" / "dispatchKeyboardMSG" hatasi verirse**
Geode sinif imzalari oyun surumune gore degisebiliyor. Hatayi bana yazarsan
kodu sana gore duzeltirim.

**Sag Shift menuyu acmiyor**
Baska bir mod klavyeyi engelliyor olabilir. Geode menusunden baska tus atamali
modlari (keybind) bir surelik kapatip dene.
