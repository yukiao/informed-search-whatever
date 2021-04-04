Informed Search Test
=====

Jenis Masalah
-----

Digunakan peta Teyvat dengan jarak sebenarnya + medan sebagai _cost_:

![](https://cdn.discordapp.com/attachments/454274817236140033/827436792612651028/Screenshot_1342.png)

Peta yang sama, dengan tingkat bahaya peta sebagai _cost_:

![](https://cdn.discordapp.com/attachments/454274817236140033/827436786636292096/Screenshot_1343.png)

Dan peta Romania dengan _cost_ sisi sebagai berikut:

![](https://user-images.githubusercontent.com/20141798/35662691-7d0dbc72-06e7-11e8-942c-4718f079bb8f.png)

Format File Graf
-----

File `teyvat.txt` dan `romania.txt` berisi definisi graf untuk peta Teyvat dan Romania masing-masing. Informasi lebih lanjut mengenai format ini
dapat dilihat pada file `teyvat.txt`.

Implementasi parsing dari format ini bisa dilihat pada `GraphInfo.cpp`.

Compile
-----

Baik compiler MSVC, Clang, maupun GCC dapat digunakan asal mendukung C++14.

MSVC:
```
cl.exe /EHsc /Fea.exe /Ox /Ot /D_CRT_SECURE_NO_WARNINGS *.cpp
```

Clang:
```
clang++ -O3 *.cpp
```

GCC:
```
g++ -O3 *.cpp
```

Akan dihasilkan executable `a.exe` (Windows) atau `a.out` (OS lain). File tersebut menerima 1 argumen angka opsional: banyak kali pengujian.
Jadi, `a.exe` akan menguji sebanyak 1 kali dan `a.exe 500` akan menguji sebanyak 500 kali.

Fungsi Heuristik
-----

Mencari jalan terpendek cukup menggunakan fungsi heuristik jarak Euclidean, yaitu:

![](https://render.githubusercontent.com/render/math?math=%5Csqrt%7B(a_x%20-%20b_x)%5E2%20%2B%20(a_y%20-%20b_y)%5E2%7D)

Namun, fungsi heuristik tersebut tidak dapat digunakan untuk mencari jalan teraman. Jalan teraman bisa lebih jauh daripada jalan terpendek.
Sebaliknya, jalan terpendek belum tentu merupakan jalan teraman. Maka digunakan fungsi heuristik berikut:

![](https://cdn.discordapp.com/attachments/454274817236140033/827444013048660000/safest_heuristic.png)

Benchmark
-----

CPU: Intel Core i5-7200U  
Compiler: g++ (Debian 8.3.0-6) 8.3.0  
OS: Windows 10 20H2 - WSL 1, Debian 10  
Runs: 100000 (seratus ribu)

Waktu dan memori serta hasil cost dan ekspansi berturut-turut:

|   Masalah/Algoritma                | Teyvat: Mondstadt - Dawn Winery (Jarak) | Teyvat: Mondstadt - Dawn Winery (Bahaya) | Teyvat: Starfell Lake - Stormterror's Lair (Jarak) | Teyvat: Starfell Lake - Stormterror's Lair (Bahaya) | Romania: Arad - Bucharest         |
| ---------------------------------- | --------------------------------------- | ---------------------------------------- | -------------------------------------------------- | --------------------------------------------------  | --------------------------------- |
| Greedy Best FirstSearch: Terendah  | 3370.76 ns, 904 bytes, **1240, 2**      | 8953.95 ns, 904 bytes, **14, 2**         | 5149.24 ns, 1288 bytes, **3068, 5**                | 19454.09 ns, 1384 bytes, 60, 5                      | 3674.42 ns, 904 bytes, 450, 3     |
| A\*: Terendah                      | 2936.85 ns, 624 bytes, **1240, 2**      | 8641.24 ns, 624 bytes, **14, 2**         | 27526.69 ns, 5952 bytes, **3068, 5**               | 120705.09 ns, 5976 bytes, 57, 6                     | 4705.88 ns, 672 bytes, **418, 4** |

Angka berhuruf tebal berarti algoritma menghasilkan solusi terbaik pada masalah tersebut.

Kesimpulan
-----

* Rata-rata, A\* memberi solusi terbaik dari uji coba.

* Fungsi heuristik yang digunakan sangat mempengaruhi algoritma A\* secara signifikan. Bisa dilihat ada perbedaan waktu signifikan pada mencari jalan teraman pada peta Teyvat.

* Fungsi heuristik juga mempengaruhi kebenaran dari hasil. Pada masalah mencari jalan teraman dari Starfell Lake ke Stormterror's Lair, hasil optimumnya adalah: Starfell Lake --> Whispering Woods --> Mondstadt --> Springvale --> Dawn Winery --> Wolvendom --> Brightcrown Canyon --> Stormterror's Lair dengan _cost_ 55.

* Best-First tidak memperhatikan cost yang sudah dicapai sehingga tidak memberi solusi terbaik pada peta Romania
