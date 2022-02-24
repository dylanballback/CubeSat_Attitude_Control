Iw = 122.18e4/1000^3; %g-mm^2 to kgm^2
Ib = 729.37e-2/1000^3; %g-mm^2 to kgm^2
Lb = 31.5/1000; %mm to m
L = 48.6/1000; %mm to m
Mb = 24/1000; %g to kg
Mw = 30/1000; %g to kg
Km = 37.5e-3; %Nm/Amp
g = 9.81; %m/s^2

M = [Iw 0;0 Ib+Mw*L^2]
K = [0 0;0 -(Mb*Lb+Mw*L)]

A = [zeros(size(M)) eye(size(M));-inv(M)*K -inv(M)*K*0];
B = [0;0;Km/Iw;-Km/(Ib+Mw*L^2)];
P1 = -max(eig(A));
P = [P1 1.1*P1 1.2*P1 1.3*P1];
Gains = place(A,B,P)

C = [0 1 0 0];

s = tf('s');

SYS = C*inv(s*eye(size(A))-A)*B;
[num,den]=tfdata(SYS,'v')
sisotool(SYS)
