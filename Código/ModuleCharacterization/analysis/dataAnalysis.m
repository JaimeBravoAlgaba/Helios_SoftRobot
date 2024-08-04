addpath('../dataset/240510');
module = '0x44';
data = readtable(strcat(module, '_240510_3.csv'));
load(strcat('minmax_', module, '_240510.mat'));
load('models_240510.mat');

switch module
    case '0x40'
        model0 = h0_40;
        model1 = h1_40;
    case '0x41'
        model0 = h0_41;
        model1 = h1_41;
    case '0x44'
        model0 = h0_45;
        model1 = h1_45;
    case '0x45'
        model0 = h0_45;
        model1 = h1_45;
    case '0x48'
        model0 = h0_48;
        model1 = h1_48;
    case '0x4A'
        model0 = h0_4A;
        model1 = h1_4A;
end

close all;
%% DATA EXTRACTION AND PREPROCESSINGh0
qx = data{:, 1}*pi/180;
qy = data{:, 2}*pi/180;
qz = data{:, 3}*pi/180;

h0 = data{:, 4};
h1 = data{:, 5};

% Filter
windowSize = 50; 
b = (1/windowSize)*ones(1,windowSize);
a = 1;

h0 = filter(b, a, h0);
h1 = filter(b, a, h1);
qx = filter(b, a, qx);
qy = filter(b, a, qy);
qz = filter(b, a, qz);

h0 = h0(windowSize:end); % Delete the first windowSize samples (filter initialization)
h1 = h1(windowSize:end);
qx = qx(windowSize:end);
qy = qy(windowSize:end);
qz = qz(windowSize:end);

figure;
set(gcf, 'Color', 'white');
subplot(1, 2, 1);
    title('Filtered [h0, h1]');
    hist([h0,h1], 50);

% Normalize
%[h0, h0_min, h0_max] = minmaxNorm(h0);
%[h1, h1_min, h1_max] = minmaxNorm(h1);
h0 = (h0-h0min)/(h0max-h0min);
h1 = (h1-h1min)/(h1max-h1min);
h0 = 2*h0-1;
h1 = 2*h1-1;

subplot(1, 2, 2);
    hist([h0,h1], 50);

theta = sqrt(qz.^2+qy.^2);
phi = atan2(qz, qy)-pi/4;
phi=wrapTo2Pi(phi);

theta0 = theta.*sin(phi);
theta1 = theta.*cos(phi);

figure; plot(h0,theta0*180/pi, '.'); hold on; plot(h1,theta1*180/pi, '.');
grid on;
xlabel('h0, h1');
ylabel('theta0, theta1 [deg]');
legend('theta 0', 'theta 1');

theta0_pred = model0(h0);
theta1_pred = model1(h1);

figure;
subplot(1, 2, 1);
    plot(h0,theta0*180/pi, '.'); hold on; plot(h0,theta0_pred*180/pi, '-');
    grid on;
    xlabel('h0');
    ylabel('theta0 [deg]');
    legend('theta 0', 'theta 0 pred');

subplot(1, 2, 2);
    plot(h1,theta1*180/pi, '.'); hold on; plot(h1,theta1_pred*180/pi, '-');
    grid on;
    xlabel('h1');
    ylabel('theta1 [deg]');
    legend('theta 1', 'theta 1 pred');

function [x_norm,m,M] = minmaxNorm(x)
    m = min(x);
    M = max(x);
    x_norm = (x-m)/(M-m);
end

function [x_norm,m,M,mm] = meanNorm(x)
    mm = mean(x);
    m = min(x);
    M = max(x);
    x_norm = (x-mm)/(M-m);
end