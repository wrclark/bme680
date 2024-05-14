# bme680/example


log.c
```
2024-05-14T13:03:02+0100 20.8916 99836.7 68.6594 7180.73 1
2024-05-14T13:04:03+0100 20.8658 99840.3 68.7844 7171.53 1
2024-05-14T13:05:03+0100 20.8979 99838.6 68.7759 7162.35 1
2024-05-14T13:06:03+0100 20.9208 99843.8 68.6436 7166.93 1
2024-05-14T13:07:03+0100 20.9303 99845.4 68.747 7144.05 1
2024-05-14T13:08:03+0100 20.9249 99846 68.7122 7139.49 0
2024-05-14T13:09:04+0100 20.9036 99845.2 68.7224 7157.76 1
2024-05-14T13:10:04+0100 20.8737 99850.5 68.575 7144.05 1
2024-05-14T13:11:04+0100 20.8753 99850.6 68.8879 7139.49 1
2024-05-14T13:12:04+0100 20.9152 99849.3 68.8602 7134.94 1
```

iterate\_setpoint.c
```
heater_target=300.0, ambient_temp=19.0
first few setpoints have heater_target=380
[Setpoint 0] temp=18.37°C, press=1005.54 mbar, hum=67.03 %RH, gas_res=10749063.67 gv=1 hs=0
[Setpoint 1] temp=18.37°C, press=1005.54 mbar, hum=67.06 %RH, gas_res=7906336.09 gv=1 hs=0
[Setpoint 2] temp=18.38°C, press=1005.54 mbar, hum=67.07 %RH, gas_res=6449438.20 gv=1 hs=0
[Setpoint 3] temp=18.38°C, press=1005.54 mbar, hum=67.06 %RH, gas_res=5329619.31 gv=1 hs=0
[Setpoint 4] temp=18.39°C, press=1005.54 mbar, hum=67.03 %RH, gas_res=4701064.70 gv=1 hs=0
[Setpoint 5] temp=18.40°C, press=1005.54 mbar, hum=66.99 %RH, gas_res=4242424.24 gv=1 hs=0
[Setpoint 6] temp=18.41°C, press=1005.54 mbar, hum=66.93 %RH, gas_res=7667.94 gv=1 hs=1
[Setpoint 7] temp=18.43°C, press=1005.54 mbar, hum=66.92 %RH, gas_res=8942.73 gv=1 hs=1
[Setpoint 8] temp=18.45°C, press=1005.54 mbar, hum=66.89 %RH, gas_res=9496.84 gv=1 hs=1
[Setpoint 9] temp=18.47°C, press=1005.54 mbar, hum=66.79 %RH, gas_res=9804.77 gv=1 hs=1
```

continuous.c
```
heater_target=300.0, ambient_temp=19.0
temp=21.54°C, press=1007.11 mbar, hum=58.33 %RH, gas_res=8019.69 gv=1 hs=1
temp=21.56°C, press=1007.11 mbar, hum=58.64 %RH, gas_res=8118.62 gv=1 hs=1
temp=21.60°C, press=1007.11 mbar, hum=58.40 %RH, gas_res=8118.62 gv=1 hs=1
temp=21.64°C, press=1007.11 mbar, hum=57.82 %RH, gas_res=8065.95 gv=1 hs=1
temp=21.69°C, press=1007.12 mbar, hum=57.05 %RH, gas_res=8025.45 gv=1 hs=1
temp=21.74°C, press=1007.12 mbar, hum=56.21 %RH, gas_res=8002.49 gv=1 hs=1
temp=21.78°C, press=1007.12 mbar, hum=55.35 %RH, gas_res=7991.05 gv=1 hs=1
temp=21.83°C, press=1007.12 mbar, hum=54.50 %RH, gas_res=7951.30 gv=1 hs=1
temp=21.89°C, press=1007.12 mbar, hum=53.66 %RH, gas_res=7940.02 gv=1 hs=1
temp=21.94°C, press=1007.13 mbar, hum=52.87 %RH, gas_res=7911.94 gv=1 hs=1
temp=21.99°C, press=1007.13 mbar, hum=52.13 %RH, gas_res=7906.35 gv=1 hs=1
temp=22.05°C, press=1007.13 mbar, hum=51.42 %RH, gas_res=7940.02 gv=1 hs=1
temp=22.10°C, press=1007.13 mbar, hum=50.73 %RH, gas_res=7934.39 gv=1 hs=1
temp=22.16°C, press=1007.14 mbar, hum=50.10 %RH, gas_res=7979.66 gv=1 hs=1
temp=22.21°C, press=1007.14 mbar, hum=49.50 %RH, gas_res=7934.39 gv=1 hs=1
temp=22.27°C, press=1007.14 mbar, hum=48.93 %RH, gas_res=8019.69 gv=1 hs=1
temp=22.33°C, press=1007.15 mbar, hum=48.40 %RH, gas_res=8002.49 gv=1 hs=1
temp=22.38°C, press=1007.15 mbar, hum=47.91 %RH, gas_res=8031.21 gv=1 hs=1
temp=22.44°C, press=1007.15 mbar, hum=47.45 %RH, gas_res=8031.21 gv=1 hs=1
temp=22.50°C, press=1007.15 mbar, hum=47.01 %RH, gas_res=7991.05 gv=1 hs=1
temp=22.56°C, press=1007.16 mbar, hum=46.59 %RH, gas_res=8002.49 gv=1 hs=1
temp=22.62°C, press=1007.16 mbar, hum=46.22 %RH, gas_res=8002.49 gv=1 hs=1
temp=22.67°C, press=1007.16 mbar, hum=45.86 %RH, gas_res=8002.49 gv=1 hs=1
temp=22.73°C, press=1007.16 mbar, hum=45.55 %RH, gas_res=8002.49 gv=1 hs=1
temp=22.79°C, press=1007.16 mbar, hum=45.23 %RH, gas_res=8002.49 gv=1 hs=1
temp=22.85°C, press=1007.17 mbar, hum=44.95 %RH, gas_res=8013.95 gv=1 hs=1
temp=22.91°C, press=1007.17 mbar, hum=44.67 %RH, gas_res=8031.21 gv=1 hs=1
temp=22.96°C, press=1007.17 mbar, hum=44.42 %RH, gas_res=8042.75 gv=1 hs=1
temp=23.02°C, press=1007.18 mbar, hum=44.18 %RH, gas_res=8025.45 gv=1 hs=1
temp=23.08°C, press=1007.18 mbar, hum=43.96 %RH, gas_res=8019.69 gv=1 hs=1
temp=23.14°C, press=1007.18 mbar, hum=43.73 %RH, gas_res=8036.98 gv=1 hs=1
temp=23.19°C, press=1007.18 mbar, hum=43.53 %RH, gas_res=8060.14 gv=1 hs=1
temp=23.25°C, press=1007.18 mbar, hum=43.34 %RH, gas_res=8008.21 gv=1 hs=1
temp=23.31°C, press=1007.18 mbar, hum=43.17 %RH, gas_res=8042.75 gv=1 hs=0
temp=23.37°C, press=1007.19 mbar, hum=42.99 %RH, gas_res=8031.21 gv=1 hs=1
temp=23.42°C, press=1007.19 mbar, hum=42.82 %RH, gas_res=8013.95 gv=1 hs=1
temp=23.48°C, press=1007.19 mbar, hum=42.68 %RH, gas_res=7991.05 gv=1 hs=1
temp=23.54°C, press=1007.19 mbar, hum=42.53 %RH, gas_res=8019.69 gv=1 hs=1
temp=23.59°C, press=1007.20 mbar, hum=42.39 %RH, gas_res=8008.21 gv=1 hs=1
temp=23.65°C, press=1007.20 mbar, hum=42.26 %RH, gas_res=8002.49 gv=1 hs=1
temp=23.70°C, press=1007.20 mbar, hum=42.14 %RH, gas_res=8002.49 gv=1 hs=1
temp=23.76°C, press=1007.20 mbar, hum=42.01 %RH, gas_res=8008.21 gv=1 hs=1
temp=23.81°C, press=1007.20 mbar, hum=41.90 %RH, gas_res=8036.98 gv=1 hs=1
temp=23.87°C, press=1007.21 mbar, hum=41.78 %RH, gas_res=8031.21 gv=1 hs=1
temp=23.93°C, press=1007.21 mbar, hum=41.68 %RH, gas_res=8031.21 gv=1 hs=0
temp=23.98°C, press=1007.21 mbar, hum=41.57 %RH, gas_res=8013.95 gv=1 hs=1
temp=24.03°C, press=1007.21 mbar, hum=41.48 %RH, gas_res=8013.95 gv=1 hs=1
temp=24.09°C, press=1007.21 mbar, hum=41.38 %RH, gas_res=7991.05 gv=1 hs=1
temp=24.14°C, press=1007.22 mbar, hum=41.30 %RH, gas_res=8002.49 gv=1 hs=1
temp=24.20°C, press=1007.22 mbar, hum=41.20 %RH, gas_res=8002.49 gv=1 hs=1
temp=24.25°C, press=1007.22 mbar, hum=41.11 %RH, gas_res=8013.95 gv=1 hs=1
temp=24.30°C, press=1007.22 mbar, hum=41.04 %RH, gas_res=8031.21 gv=1 hs=1
temp=24.36°C, press=1007.22 mbar, hum=40.95 %RH, gas_res=8025.45 gv=1 hs=1
temp=24.41°C, press=1007.22 mbar, hum=40.87 %RH, gas_res=8008.21 gv=1 hs=1
temp=24.46°C, press=1007.23 mbar, hum=40.80 %RH, gas_res=8008.21 gv=1 hs=1
temp=24.52°C, press=1007.23 mbar, hum=40.73 %RH, gas_res=7973.97 gv=1 hs=1
temp=24.57°C, press=1007.23 mbar, hum=40.67 %RH, gas_res=7996.77 gv=1 hs=1
temp=24.62°C, press=1007.23 mbar, hum=40.61 %RH, gas_res=8036.98 gv=1 hs=1
temp=24.67°C, press=1007.23 mbar, hum=40.55 %RH, gas_res=7996.77 gv=1 hs=1
temp=24.72°C, press=1007.24 mbar, hum=40.49 %RH, gas_res=8008.21 gv=1 hs=1
temp=24.77°C, press=1007.24 mbar, hum=40.44 %RH, gas_res=7991.05 gv=1 hs=1
temp=24.83°C, press=1007.24 mbar, hum=40.37 %RH, gas_res=7979.66 gv=1 hs=1
temp=24.88°C, press=1007.24 mbar, hum=40.32 %RH, gas_res=7968.29 gv=1 hs=1
temp=24.93°C, press=1007.24 mbar, hum=40.26 %RH, gas_res=7973.97 gv=1 hs=1
temp=24.98°C, press=1007.24 mbar, hum=40.21 %RH, gas_res=7985.35 gv=1 hs=1
temp=25.03°C, press=1007.25 mbar, hum=40.15 %RH, gas_res=7991.05 gv=1 hs=1
temp=25.08°C, press=1007.25 mbar, hum=40.10 %RH, gas_res=7968.29 gv=1 hs=1
temp=25.13°C, press=1007.25 mbar, hum=40.05 %RH, gas_res=7996.77 gv=1 hs=1
temp=25.17°C, press=1007.25 mbar, hum=39.99 %RH, gas_res=8008.21 gv=1 hs=1
temp=25.22°C, press=1007.25 mbar, hum=39.94 %RH, gas_res=7968.29 gv=1 hs=1
temp=25.27°C, press=1007.25 mbar, hum=39.90 %RH, gas_res=7973.97 gv=1 hs=1
temp=25.32°C, press=1007.26 mbar, hum=39.85 %RH, gas_res=7985.35 gv=1 hs=1
temp=25.37°C, press=1007.26 mbar, hum=39.81 %RH, gas_res=7973.97 gv=1 hs=1
temp=25.42°C, press=1007.26 mbar, hum=39.78 %RH, gas_res=8002.49 gv=1 hs=1
temp=25.46°C, press=1007.26 mbar, hum=39.73 %RH, gas_res=7979.66 gv=1 hs=1
temp=25.51°C, press=1007.26 mbar, hum=39.70 %RH, gas_res=7951.30 gv=1 hs=1
temp=25.56°C, press=1007.26 mbar, hum=39.66 %RH, gas_res=7951.30 gv=1 hs=1
temp=25.60°C, press=1007.26 mbar, hum=39.63 %RH, gas_res=7956.96 gv=1 hs=1
temp=25.65°C, press=1007.27 mbar, hum=39.59 %RH, gas_res=7973.97 gv=1 hs=1
temp=25.70°C, press=1007.27 mbar, hum=39.55 %RH, gas_res=7968.29 gv=1 hs=1
temp=25.74°C, press=1007.27 mbar, hum=39.52 %RH, gas_res=7962.62 gv=1 hs=1
temp=25.79°C, press=1007.27 mbar, hum=39.49 %RH, gas_res=7962.62 gv=1 hs=1
temp=25.83°C, press=1007.27 mbar, hum=39.45 %RH, gas_res=7951.30 gv=1 hs=1
temp=25.88°C, press=1007.27 mbar, hum=39.41 %RH, gas_res=7962.62 gv=1 hs=1
temp=25.92°C, press=1007.27 mbar, hum=39.38 %RH, gas_res=7968.29 gv=1 hs=1
temp=25.97°C, press=1007.28 mbar, hum=39.34 %RH, gas_res=7962.62 gv=1 hs=1
temp=26.01°C, press=1007.28 mbar, hum=39.31 %RH, gas_res=7956.96 gv=1 hs=1
temp=26.06°C, press=1007.28 mbar, hum=39.27 %RH, gas_res=7956.96 gv=1 hs=1
temp=26.10°C, press=1007.28 mbar, hum=39.22 %RH, gas_res=7951.30 gv=1 hs=1
temp=26.15°C, press=1007.28 mbar, hum=39.20 %RH, gas_res=7951.30 gv=1 hs=1
temp=26.19°C, press=1007.28 mbar, hum=39.15 %RH, gas_res=7934.39 gv=1 hs=1
temp=26.23°C, press=1007.28 mbar, hum=39.13 %RH, gas_res=7923.15 gv=1 hs=1
temp=26.28°C, press=1007.28 mbar, hum=39.10 %RH, gas_res=7940.02 gv=1 hs=1
temp=26.32°C, press=1007.28 mbar, hum=39.07 %RH, gas_res=7928.76 gv=1 hs=1
temp=26.36°C, press=1007.28 mbar, hum=39.04 %RH, gas_res=7917.54 gv=1 hs=1
temp=26.40°C, press=1007.28 mbar, hum=39.02 %RH, gas_res=7934.39 gv=1 hs=1
temp=26.44°C, press=1007.28 mbar, hum=39.01 %RH, gas_res=7945.66 gv=1 hs=1
temp=26.49°C, press=1007.29 mbar, hum=38.98 %RH, gas_res=7968.29 gv=1 hs=1
temp=26.53°C, press=1007.29 mbar, hum=38.96 %RH, gas_res=7945.66 gv=1 hs=1
temp=26.57°C, press=1007.29 mbar, hum=38.94 %RH, gas_res=7945.66 gv=1 hs=1
temp=26.61°C, press=1007.29 mbar, hum=38.92 %RH, gas_res=7934.39 gv=1 hs=1
```

