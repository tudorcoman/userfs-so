<h1 align="center">Welcome to UserFS 👋</h1>
<p>
  <a href="#" target="_blank">
    <img alt="License: UniBuc" src="https://img.shields.io/badge/License-UniBuc-yellow.svg" />
  </a>
</p>

> Am realizat un pseudo-sistem de fisiere care sa afiseze utilizatorii activi din sistem si procesele asociate. Cand este montat, in radacina se vor gasi directoare corespunzatoare fiecarui utilizator activ. In fiecare director se va gasi un fisier PROCS ce contine lista aferenta de procese active.

### 🏠 [Homepage](https://github.com/tudorcoman/userfs-so)

## Install

```sh
git clone https://github.com/tudorcoman/userfs-so
```

## Usage

```sh
make clean all
make
./userfs -s mountpoint
```

## Run tests

```sh
ls -l mountpoint
ls -l mountpoint/username
cat mountpoint/username/procs
```

## Author

👤 **Chiriac Ella-Ana-Maria, Coman Tudor, Stoica Elias-Valeriu**

* Github: [@ella2222](https://github.com/ella2222), [@tudorcoman](https://github.com/tudorcoman), @[wood11nho](https://github.com/wood11nho)