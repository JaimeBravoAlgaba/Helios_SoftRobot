addpath('../dataset/240510');
dataset = '240510_3';
load(strcat('h_', dataset, '.mat'));

% Filter
windowSize = 50; 
b = (1/windowSize)*ones(1,windowSize);
a = 1;

h0_40 = filter(b, a, h0_40);
h0_41 = filter(b, a, h0_41);
%h0_44 = filter(b, a, h0_44);
h0_45 = filter(b, a, h0_45);
h0_48 = filter(b, a, h0_48);
h0_4A = filter(b, a, h0_4A);

h1_40 = filter(b, a, h1_40);
h1_41 = filter(b, a, h1_41);
%h1_44 = filter(b, a, h1_44);
h1_45 = filter(b, a, h1_45);
h1_48 = filter(b, a, h1_48);
h1_4A = filter(b, a, h1_4A);

% Delete the first windowSize samples
h0_40 = h0_40(windowSize:end);
h0_41 = h0_41(windowSize:end);
%h0_44 = h0_44(windowSize:end);
h0_45 = h0_45(windowSize:end);
h0_48 = h0_48(windowSize:end);
h0_4A = h0_4A(windowSize:end);

h1_40 = h1_40(windowSize:end);
h1_41 = h1_41(windowSize:end);
%h1_44 = h1_44(windowSize:end);
h1_45 = h1_45(windowSize:end);
h1_48 = h1_48(windowSize:end);
h1_4A = h1_4A(windowSize:end);

% Generate the histogram
figure;
set(gcf, 'Name', dataset, 'Color', 'white');
subplot(1, 2, 1);
    N_40 = hist(h0_40, 50);
    N_41 = hist(h0_41, 50);
    %N_44 = hist(h0_44, 100);
    N_45 = hist(h0_45, 50);
    N_48 = hist(h0_48, 50);
    N_4A = hist(h0_4A, 50);

    % Percentage
    P_40 = N_40 / sum(N_40);
    P_41 = N_41 / sum(N_41);
    %P_44 = N_44 / sum(N_44);
    P_45 = N_45 / sum(N_45);
    P_48 = N_48 / sum(N_48);
    P_4A = N_4A / sum(N_4A);

    plot(0.02:0.02:1, 100*[P_40; P_41; P_45; P_48; P_4A]', 'LineWidth', 2);

    title('h0');
    xlabel('Value');
    ylabel('Frequency [%]');
    legend('h0_{40}', 'h0_{41}', 'h0_{45}', 'h0_{48}', 'h0_{4A}');
    ylim([0 15]);
    yticks(0:3:15);
    grid on;

subplot(1, 2, 2);
    N_40 = hist(h1_40, 50);
    N_41 = hist(h1_41, 50);
    %N_44 = hist(h1_44, 100);
    N_45 = hist(h1_45, 50);
    N_48 = hist(h1_48, 50);
    N_4A = hist(h1_4A, 50);

    % Percentage
    P_40 = N_40 / sum(N_40);
    P_41 = N_41 / sum(N_41);
    %P_44 = N_44 / sum(N_44);
    P_45 = N_45 / sum(N_45);
    P_48 = N_48 / sum(N_48);
    P_4A = N_4A / sum(N_4A);

    plot(0.02:0.02:1, 100*[P_40; P_41; P_45; P_48; P_4A]', 'LineWidth', 2);

    title('h1');
    xlabel('Value');
    ylabel('Frequency [%]');
    legend('h1_{40}', 'h1_{41}', 'h1_{45}', 'h1_{48}', 'h1_{4A}');
    ylim([0 15]);
    yticks(0:3:15);
    grid on;


