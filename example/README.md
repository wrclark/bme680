# bme680/example


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
temp=20.73°C, press=1005.41 mbar, hum=54.19 %RH, gas_res=9537.32
temp=20.74°C, press=1005.41 mbar, hum=54.42 %RH, gas_res=9813.37
temp=20.75°C, press=1005.41 mbar, hum=54.61 %RH, gas_res=9813.37
temp=20.78°C, press=1005.41 mbar, hum=54.76 %RH, gas_res=9804.77
temp=20.80°C, press=1005.41 mbar, hum=54.87 %RH, gas_res=9813.37
temp=20.83°C, press=1005.41 mbar, hum=54.95 %RH, gas_res=9787.61
temp=20.85°C, press=1005.41 mbar, hum=54.99 %RH, gas_res=9770.52
temp=20.88°C, press=1005.42 mbar, hum=55.01 %RH, gas_res=9770.52
temp=20.91°C, press=1005.42 mbar, hum=55.00 %RH, gas_res=9744.99
temp=20.94°C, press=1005.42 mbar, hum=54.97 %RH, gas_res=9711.16
temp=20.98°C, press=1005.42 mbar, hum=54.91 %RH, gas_res=9711.16
temp=21.01°C, press=1005.42 mbar, hum=54.84 %RH, gas_res=9694.33
temp=21.04°C, press=1005.42 mbar, hum=54.76 %RH, gas_res=9685.94
temp=21.07°C, press=1005.42 mbar, hum=54.67 %RH, gas_res=9677.56
temp=21.11°C, press=1005.42 mbar, hum=54.56 %RH, gas_res=9669.20
temp=21.14°C, press=1005.42 mbar, hum=54.45 %RH, gas_res=9669.20
temp=21.18°C, press=1005.42 mbar, hum=54.34 %RH, gas_res=9660.85
temp=21.21°C, press=1005.42 mbar, hum=54.22 %RH, gas_res=9644.19
temp=21.24°C, press=1005.43 mbar, hum=54.09 %RH, gas_res=9652.51
temp=21.28°C, press=1005.43 mbar, hum=53.96 %RH, gas_res=9644.19
temp=21.31°C, press=1005.43 mbar, hum=53.83 %RH, gas_res=9627.59
temp=21.35°C, press=1005.43 mbar, hum=53.70 %RH, gas_res=9611.05
temp=21.38°C, press=1005.43 mbar, hum=53.55 %RH, gas_res=9619.32
temp=21.42°C, press=1005.43 mbar, hum=53.42 %RH, gas_res=9619.32
temp=21.45°C, press=1005.43 mbar, hum=53.30 %RH, gas_res=9611.05
temp=21.49°C, press=1005.43 mbar, hum=53.16 %RH, gas_res=9602.80
temp=21.52°C, press=1005.44 mbar, hum=53.02 %RH, gas_res=9586.35
temp=21.56°C, press=1005.44 mbar, hum=52.90 %RH, gas_res=9602.80
temp=21.59°C, press=1005.44 mbar, hum=52.76 %RH, gas_res=9569.95
temp=21.63°C, press=1005.44 mbar, hum=52.62 %RH, gas_res=9586.35
temp=21.66°C, press=1005.44 mbar, hum=52.51 %RH, gas_res=9569.95
temp=21.70°C, press=1005.44 mbar, hum=52.38 %RH, gas_res=9569.95
temp=21.73°C, press=1005.44 mbar, hum=52.26 %RH, gas_res=9561.77
temp=21.77°C, press=1005.44 mbar, hum=52.13 %RH, gas_res=9553.61
temp=21.80°C, press=1005.45 mbar, hum=52.00 %RH, gas_res=9553.61
temp=21.84°C, press=1005.45 mbar, hum=51.89 %RH, gas_res=9553.61
temp=21.87°C, press=1005.45 mbar, hum=51.77 %RH, gas_res=9553.61
temp=21.91°C, press=1005.45 mbar, hum=51.65 %RH, gas_res=9553.61
temp=21.94°C, press=1005.45 mbar, hum=51.55 %RH, gas_res=9537.32
temp=21.98°C, press=1005.45 mbar, hum=51.41 %RH, gas_res=9521.09
temp=22.01°C, press=1005.45 mbar, hum=51.31 %RH, gas_res=9529.20
temp=22.04°C, press=1005.45 mbar, hum=51.19 %RH, gas_res=9529.20
temp=22.08°C, press=1005.45 mbar, hum=51.08 %RH, gas_res=9529.20
temp=22.11°C, press=1005.45 mbar, hum=50.97 %RH, gas_res=9512.99
temp=22.15°C, press=1005.46 mbar, hum=50.86 %RH, gas_res=9504.91
temp=22.18°C, press=1005.46 mbar, hum=50.76 %RH, gas_res=9504.91
temp=22.22°C, press=1005.46 mbar, hum=50.64 %RH, gas_res=9512.99
temp=22.25°C, press=1005.46 mbar, hum=50.54 %RH, gas_res=9504.91
temp=22.28°C, press=1005.46 mbar, hum=50.44 %RH, gas_res=9488.79
temp=22.32°C, press=1005.46 mbar, hum=50.34 %RH, gas_res=9488.79
temp=22.35°C, press=1005.46 mbar, hum=50.24 %RH, gas_res=9480.75
temp=22.39°C, press=1005.46 mbar, hum=50.15 %RH, gas_res=9480.75
temp=22.42°C, press=1005.46 mbar, hum=50.05 %RH, gas_res=9472.72
temp=22.45°C, press=1005.47 mbar, hum=49.94 %RH, gas_res=9464.71
temp=22.49°C, press=1005.47 mbar, hum=49.86 %RH, gas_res=9472.72
temp=22.52°C, press=1005.47 mbar, hum=49.76 %RH, gas_res=9464.71
temp=22.55°C, press=1005.47 mbar, hum=49.67 %RH, gas_res=9480.75
temp=22.59°C, press=1005.47 mbar, hum=49.58 %RH, gas_res=9456.71
temp=22.62°C, press=1005.47 mbar, hum=49.49 %RH, gas_res=9472.72
temp=22.65°C, press=1005.47 mbar, hum=49.41 %RH, gas_res=9456.71
temp=22.68°C, press=1005.47 mbar, hum=49.33 %RH, gas_res=9456.71
temp=22.72°C, press=1005.47 mbar, hum=49.24 %RH, gas_res=9432.79
temp=22.75°C, press=1005.47 mbar, hum=49.15 %RH, gas_res=9432.79
temp=22.78°C, press=1005.47 mbar, hum=49.08 %RH, gas_res=9440.75
temp=22.81°C, press=1005.47 mbar, hum=49.00 %RH, gas_res=9440.75
temp=22.85°C, press=1005.48 mbar, hum=48.92 %RH, gas_res=9432.79
temp=22.88°C, press=1005.48 mbar, hum=48.84 %RH, gas_res=9432.79
temp=22.91°C, press=1005.48 mbar, hum=48.77 %RH, gas_res=9416.91
temp=22.94°C, press=1005.48 mbar, hum=48.71 %RH, gas_res=9440.75
temp=22.97°C, press=1005.48 mbar, hum=48.63 %RH, gas_res=9440.75
temp=23.00°C, press=1005.48 mbar, hum=48.56 %RH, gas_res=9432.79
temp=23.04°C, press=1005.48 mbar, hum=48.50 %RH, gas_res=9408.99
temp=23.07°C, press=1005.48 mbar, hum=48.42 %RH, gas_res=9424.84
temp=23.10°C, press=1005.48 mbar, hum=48.36 %RH, gas_res=9416.91
temp=23.13°C, press=1005.48 mbar, hum=48.29 %RH, gas_res=9408.99
temp=23.16°C, press=1005.48 mbar, hum=48.24 %RH, gas_res=9408.99
temp=23.19°C, press=1005.48 mbar, hum=48.17 %RH, gas_res=9408.99
temp=23.22°C, press=1005.48 mbar, hum=48.12 %RH, gas_res=9416.91
temp=23.25°C, press=1005.48 mbar, hum=48.05 %RH, gas_res=9416.91
temp=23.28°C, press=1005.49 mbar, hum=48.00 %RH, gas_res=9416.91
temp=23.31°C, press=1005.49 mbar, hum=47.94 %RH, gas_res=9401.09
temp=23.34°C, press=1005.49 mbar, hum=47.89 %RH, gas_res=9385.32
temp=23.37°C, press=1005.49 mbar, hum=47.84 %RH, gas_res=9385.32
temp=23.40°C, press=1005.49 mbar, hum=47.78 %RH, gas_res=9401.09
temp=23.43°C, press=1005.49 mbar, hum=47.74 %RH, gas_res=9393.19
temp=23.46°C, press=1005.49 mbar, hum=47.68 %RH, gas_res=9385.32
temp=23.49°C, press=1005.49 mbar, hum=47.64 %RH, gas_res=9377.45
temp=23.52°C, press=1005.49 mbar, hum=47.58 %RH, gas_res=9385.32
temp=23.54°C, press=1005.49 mbar, hum=47.54 %RH, gas_res=9377.45
temp=23.57°C, press=1005.49 mbar, hum=47.49 %RH, gas_res=9385.32
temp=23.60°C, press=1005.49 mbar, hum=47.45 %RH, gas_res=9385.32
temp=23.63°C, press=1005.49 mbar, hum=47.40 %RH, gas_res=9377.45
temp=23.66°C, press=1005.49 mbar, hum=47.36 %RH, gas_res=9369.60
temp=23.69°C, press=1005.49 mbar, hum=47.32 %RH, gas_res=9361.76
temp=23.71°C, press=1005.49 mbar, hum=47.28 %RH, gas_res=9377.45
temp=23.74°C, press=1005.49 mbar, hum=47.24 %RH, gas_res=9369.60
temp=23.77°C, press=1005.49 mbar, hum=47.20 %RH, gas_res=9377.45
temp=23.80°C, press=1005.50 mbar, hum=47.16 %RH, gas_res=9385.32
temp=23.82°C, press=1005.50 mbar, hum=47.13 %RH, gas_res=9377.45
temp=23.85°C, press=1005.50 mbar, hum=47.08 %RH, gas_res=9353.93
temp=23.88°C, press=1005.50 mbar, hum=47.04 %RH, gas_res=9353.93
```
