# MacroSlots - GitHub ile Derleme (Alan Gerektirmez)

Bilgisayarinda Visual Studio / CMake **kurmana gerek yok**. GitHub'in bedava
sunucularinda modu senin icin derleyip `.geode` dosyasini indirmene izin verir.
Bilgisayarinda sadece tarayici yeterli.

> GitHub'in bedava hesabi ayda 2000 dakika islem sunar. Bu mod 2-3 dakikada
> biter, yani ayda ~700 kez derleyebilirsin. Fazlasi zaten gerekmez.

---

## 1) GitHub hesabi ac (varsa atla)

1. <https://github.com> ac -> **Sign up**
2. E-posta, sifre, kullanici adi gir -> hesabini olustur
3. E-postana gelen dogrulama baglantisina tikla

> Arayuz Ingilizceyi zor geliyorsa: giris yapiktan sonra sag ustteki profil ->
> **Settings** -> **Appearance** -> **Language** -> **Turkce** sec.

---

## 2) Yeni bir depo (repo) olustur

1. GitHub'da giris yap, sag ustte **+** isaretine tikla -> **New repository**
2. **Repository name** kismina: `MacroSlots` yaz
3. **Private** sec (sadece sen gorursun)
4. Asagidaki kutularin hicbirini isaretleme (README vs. bos birak)
5. **Create repository** butonuna bas

Artik bos bir repo'n var.

---

## 3) Proje dosyalarini yukle

1. Yeni acilan repo sayfasinda ortada su yaziyi goreceksin:
   *"...or push an existing repository from the command line"*
   biraz asagida **"uploading an existing file"** yazan mavi baglantiya tikla.

   (Yoksa ustteki **<> Code** butonuna tikla -> **uploading an existing file**)

2. Dosya yukleme ekraninda, kendi bilgisayarinda su klasoru ac:

   ```
   C:\Users\test\Documents\Varsayilan Proje\MacroSlots
   ```

3. Icindeki **her seyi** (klasorler dahil) surukle-birak yap:

   - `CMakeLists.txt`
   - `mod.json`
   - `about.md`
   - `README.md`
   - `KURULUM.md`
   - `GITHUB-DERLEME.md`
   - `src` klasoru (icindeki 6 dosyayla birlikte)
   - `.github` klasoru (icinde `workflows\build.yml` var) -
     bu gizli klasor olabilir, gosteren secenegi acmayi unutma:
     Klasor Secicende ustte **"Gizli ogeler"** kutusunu isaretle.

4. Tumu yuklendikten sonra asagidaki **Commit changes** butonuna bas.

---

## 4) Derlemeyi calistir

1. Repo sayfasinda ust menuden **Actions** sekmesine git
2. Sol tarafta **"Modu Derle (MacroSlots)"** yazisina tikla
3. Asagida olusan islem varsa tikla; henuz yoksa sagdaki **Run workflow**
   butonuna bas (yeşil buton)
4. Derleme baslar (sari nokta = calisiyor, yesil tik = bitti)
5. 2-3 dakika bekle. Yeşil tik gorunce isin bitti demektir.

---

## 5) Hazir .geode dosyasini indir

1. Actions sayfasinda biten isleme tikla
2. En asagida **Artifacts** bolumunu goreceksin
3. **MacroSlots** yazan baglantiya tikla -> `MacroSlots.zip` iner
4. Zip'i ac -> icinden **`MacroSlots.geode`** dosyasi cikar
5. Bunu oyuna kur:

   Geometry Dash'i ac -> Geode menusu -> **Mods** -> **"Install from file"**
   -> `MacroSlots.geode` dosyasini sec -> oyunu yeniden baslat.

Bitti! Artik **Sag Shift** ile menun acilir.

---

## Sonraki kullanimlar

- Proje dosyalarinda degisiklik yapinca (veya sadece GitHub'a yukleyince)
  derleme **otomatik** baslar.
- Her yeni sürum icin ayni **Actions** -> **Artifacts** yolundan indir.

---

## Sorun giderme

**Actions sekmesi bos / hic islem yok**
Dosyalari yuklerken `.github` klasorunu atlamissin demektir.
Repo sayfasinda **Add file -> Create new file** de, ustteki kutuya su yolu yaz:

```
.github/workflows/build.yml
```

(slash yazinca klasor yolu olusur). Sonra su icerigi birebir yapistir ve
**Commit changes** de. Bu tek dosya derlemeyi tetikler:

```yaml
name: Modu Derle (MacroSlots)

on:
  push:
  workflow_dispatch:

jobs:
  build:
    name: Windows
    runs-on: windows-latest

    steps:
      - uses: actions/checkout@v7

      - name: Modu derle
        uses: geode-sdk/build-geode-mod@main
        id: build

      - name: .geode dosyasini sakla
        uses: actions/upload-artifact@v7
        with:
          name: MacroSlots
          path: ${{ steps.build.outputs.build-output }}
```

**Islem kirmizi ✗ ile bitti**
Isleme tikla, hangi adimda patladigini goreceksin. "Modu derle" adimina tikla,
kirmizi yazilari sec-kopyala ve bana gonder. Sorunu kod tarafindan duzeltirim.

**"geode" surum hatasi / SDK surumu uymuyor**
`mod.json` icindeki `"geode": "v5.0.0"` yazan yeri GitHub'da duzeltebilirsin:
`mod.json` dosyasina tikla -> sag ustteki kalem (pencil) ikonu -> surumu
duzelt -> **Commit changes**. Derleme tekrar baslar.

**"wrong game version" hatasi (oyunda)**
`mod.json` icinde `"gd": { "win": "2.2091" }` kismi oyun surumunle ayni olmali.
Geode menusunde oyunun surumu yazar. Oraya gore duzelt, tekrar derle.
