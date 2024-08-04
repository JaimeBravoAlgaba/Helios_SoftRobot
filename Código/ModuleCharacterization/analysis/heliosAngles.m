addpath('../dataset/240510');
dataset = '240510_3';
load(strcat('angles_', dataset, '.mat'));

% Filter
windowSize = 50;
b = (1/windowSize)*ones(1,windowSize);
a = 1;

th_40 = filter(b, a, th_40);
th_41 = filter(b, a, th_41);
%th_44 = filter(b, a, th_44);
th_45 = filter(b, a, th_45);
th_48 = filter(b, a, th_48);
th_4A = filter(b, a, th_4A);

ph_40 = filter(b, a, ph_40);
ph_41 = filter(b, a, ph_41);
%ph_44 = filter(b, a, ph_44);
ph_45 = filter(b, a, ph_45);
ph_48 = filter(b, a, ph_48);
ph_4A = filter(b, a, ph_4A);

% Delete the first windowSize samples
th_40 = th_40(windowSize:end);
th_41 = th_41(windowSize:end);
%th_44 = th_44(windowSize:end);
th_45 = th_45(windowSize:end);
th_48 = th_48(windowSize:end);
th_4A = th_4A(windowSize:end);

ph_40 = ph_40(windowSize:end);
ph_41 = ph_41(windowSize:end);
%ph_44 = ph_44(windowSize:end);
ph_45 = ph_45(windowSize:end);
ph_48 = ph_48(windowSize:end);
ph_4A = ph_4A(windowSize:end);

% 3D plot
figure;
set(gcf, 'Name', dataset, 'Color', 'white');
subplot(1, 2, 1);
    plot3(h0_40, h1_40, th_40, '-');
    hold on;
    plot3(h0_41, h1_41, th_41, '-');
    %plot3(h0_44, h1_44, th_44, '-');
    plot3(h0_45, h1_45, th_45, '-');
    plot3(h0_48, h1_48, th_48, '-');
    plot3(h0_4A, h1_4A, th_4A, '-');
    hold off;
    title('Theta');
    xlabel('h0');
    ylabel('h1');
    zlabel('Theta');
    legend('40', '41', '45', '48', '4A');
    grid on;

subplot(1, 2, 2);
    plot3(h0_40, h1_40, ph_40, '-');
    hold on;
    plot3(h0_41, h1_41, ph_41, '-');
    %plot3(h0_44, h1_44, ph_44, '-');
    plot3(h0_45, h1_45, ph_45, '-');
    plot3(h0_48, h1_48, ph_48, '-');
    plot3(h0_4A, h1_4A, ph_4A, '-');
    hold off;
    title('Phi');
    xlabel('h0');
    ylabel('h1');
    zlabel('Phi');
    legend('40', '41', '45', '48', '4A');
    grid on;


