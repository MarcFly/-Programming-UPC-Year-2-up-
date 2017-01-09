%%%%%%%%%%% TESTS for the function ownGauss_mod.m %%%%%%%%%%%
clc;
clear all;
close all;

load('matTests.mat');

tp = 0;

r1 = A1\b1 - ownGauss_mod(A1,b1);

if max(abs(r1))<1E-8
    tp = tp +1;
end
    
r2 = A2\b2 - ownGauss_mod(A2,b2);

if max(abs(r2))<1E-6
    tp = tp +1;
end

[~, flag3] =  ownGauss_mod(A3,b3);
if flag3 == -1
    tp = tp +1;
end

[~, flag4] =  ownGauss_mod(A4,b4);
if flag4 == -1
    tp = tp +1;
end


disp('----------------TESTS for the function ownGauss_mod.m----------------');
s = sprintf('--------------------------%g%% tests passed--------------------------',tp*25);
disp(s)