% Load data from csv file
data = readtable('dataset/0x44_240428_1.csv');

% Extract data
theta = data{:, 1};
phi = data{:, 2};
h = data{:, 3:end};

theta0 = theta .* cos(phi);
theta1 = theta .* sin(phi);

%% PLOT theta0 vs h0, h2 and theta1 vs h1, h3
figure;
ax = subplot(3, 1, 1);
    plot(theta0, [h(:, 1), h(:, 3)], '.');
    title('theta0 vs (h0, h2)');
    legend('h0', 'h2');
    xlabel('theta0');
    ylabel('h');
    grid on;
    
ax = subplot(3, 1, 2);
    plot(theta1, [h(:, 2), h(:, 4)], '.');
    title('theta1 vs (h1, h3)');
    legend('h1', 'h3');
    xlabel('theta1');
    ylabel('h');
    grid on;

ax = subplot(3, 1, 3);
    plot(theta0, h(:, 1) - h(:, 3), '.');
    title('theta0 vs (h0-h2)');
    legend('h0-h2');
    xlabel('theta0');
    ylabel('h');
    grid on;

% Fit a model to the data by least squares
H0 = fit([theta0, theta1], [h(:, 1), h(:, 2)], 'poly11');