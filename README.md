# 3sem_HW1
### Тесты
impl                      n    create_ms   destroy_ms     rss_kb
raw                      10        0.001        0.000        324
UnqPtr                   10        0.001        0.000        324
ShrdPtr                  10        0.000        0.000        324
std::unique_ptr          10        0.001        0.000        324
std::shared_ptr          10        0.000        0.000        324

raw                     100        0.002        0.001        324
UnqPtr                  100        0.002        0.001        324
ShrdPtr                 100        0.003        0.002        324
std::unique_ptr         100        0.002        0.001        324
std::shared_ptr         100        0.003        0.002        324

raw                    1000        0.089        0.024        352
UnqPtr                 1000        0.031        0.010        352
ShrdPtr                1000        0.169        0.044        392
std::unique_ptr        1000        0.031        0.010        352
std::shared_ptr        1000        0.167        0.046        392

raw                   10000        0.529        0.219        496
UnqPtr                10000        0.447        0.123        496
ShrdPtr               10000        0.864        0.199        940
std::unique_ptr       10000        0.793        0.216        496
std::shared_ptr       10000        0.844        0.205        940

raw                  100000        4.261        0.968       3952
UnqPtr               100000        3.751        1.026       3952
ShrdPtr              100000        8.074        1.985       7920
std::unique_ptr      100000        4.797        1.010       3952
std::shared_ptr      100000        8.885        2.273       7664

raw                 1000000       34.979        9.870      39056
UnqPtr              1000000       35.348        9.870      38948
ShrdPtr             1000000       69.806       19.920      78040
std::unique_ptr     1000000       35.103        9.881      39080
std::shared_ptr     1000000       70.318       20.606      78044


### Valgrind тесты
==6363== Memcheck, a memory error detector
==6363== Copyright (C) 2002-2024, and GNU GPL'd, by Julian Seward et al.
==6363== Using Valgrind-3.24.0 and LibVEX; rerun with -h for copyright info
==6363== Command: ./build/testUnqPtr
==6363== 
==6363== 
==6363== HEAP SUMMARY:
==6363==     in use at exit: 0 bytes in 0 blocks
==6363==   total heap usage: 466 allocs, 466 frees, 147,770 bytes allocated
==6363== 
==6363== All heap blocks were freed -- no leaks are possible
==6363== 
==6363== For lists of detected and suppressed errors, rerun with: -s
==6363== ERROR SUMMARY: 0 errors from 0 contexts (suppressed: 0 from 0)


==6492== Memcheck, a memory error detector
==6492== Copyright (C) 2002-2024, and GNU GPL'd, by Julian Seward et al.
==6492== Using Valgrind-3.24.0 and LibVEX; rerun with -h for copyright info
==6492== Command: ./build/testShrdPtr
==6492== 
==6492== 
==6492== HEAP SUMMARY:
==6492==     in use at exit: 0 bytes in 0 blocks
==6492==   total heap usage: 565 allocs, 565 frees, 1,152,439 bytes allocated
==6492== 
==6492== All heap blocks were freed -- no leaks are possible
==6492== 
==6492== For lists of detected and suppressed errors, rerun with: -s
==6492== ERROR SUMMARY: 0 errors from 0 contexts (suppressed: 0 from 0)
