from cmath import pi, tan, atan, sqrt

V_in_min = 3
V_out = 5
I_load = 2
L = 3.3e-6
eta = 0.77
C_out = 150e-6
G_CS = 10.8
phi_P2 = pi/2

V_ref = 0.795
g_m = 120e-6
R_EA = 5e6


R_load = V_out/I_load
f_RHPZ = R_load/(2*pi*L)*(V_in_min/V_out)**2
f_CC = f_RHPZ/6
phi_RHPZ = atan(f_CC/f_RHPZ)
G_CS_out = G_CS*V_in_min/V_out*eta
f_P1 = 1/(2*pi*R_load*C_out)
G_out = G_CS_out*sqrt(R_load**2/(1 + (f_CC/f_P1)**2))
phi_P1 = atan(f_CC/f_P1)
phi_Z1 = 50/180*pi + phi_P2 + phi_P1 + phi_RHPZ - pi
G_comp = 1/(V_ref/V_out*G_out)
R_Z = G_comp/g_m
C_P1 = tan(phi_Z1)/(2*pi*f_CC*R_Z)
f_P2 = 1/(2*pi*R_EA*C_P1)

print('f_RHPZ:      ', f_RHPZ)
print('phi_RHPZ:    ', phi_RHPZ*180/pi, ' Deg')
print('f_P1:        ', f_P1)
print('phi_P1:      ', phi_P1*180/pi, ' Deg')
print('phi_Z1:      ', phi_Z1*180/pi, ' Deg')
print('R_Z:         ', R_Z)
print('C_P1:        ', C_P1)
print('f_P2:        ', f_P2)