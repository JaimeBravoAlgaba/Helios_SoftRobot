% Load data from csv file
addpath('..\');
data = readtable('dataset/0x4A_240423_1.csv');
data_validation = readtable('dataset/0x4A_240423_1.csv');
modelname = 'model_0x4A.mat';

% Extract data
theta = data{:, 1} * pi / 180;
phi = data{:, 2} * pi / 180;
for i = 1:length(phi)
    if theta(i) < 0
        theta(i) = -theta(i);
        phi(i) = phi(i) - pi;
    end

    if phi(i) < 0
        phi(i) = phi(i) + 2*pi;
    end
end

x0 = data{:, 3};
x1 = data{:, 4};

y0 = theta .* cos(phi);
y1 = theta .* sin(phi);
ym = mean([y0, y1], 2);

% Moving average filter
window_size = 10;
b = (1/window_size) * ones(1, window_size);
a = 1;
y0_filt = filter(b, a, y0);
y1_filt = filter(b, a, y1);

x0_filt = filter(b, a, x0);
x1_filt = filter(b, a, x1);

% Plot original data vs filtered
figure;
subplot(2, 2, 1);
    plot(x0, 'r');
    hold on;
    plot(x0_filt, 'b');
    title('x0');
    legend('Original', 'Filtered');

subplot(2, 2, 2);
    plot(x1, 'r');
    hold on;
    plot(x1_filt, 'b');
    title('x1');
    legend('Original', 'Filtered');

subplot(2, 2, 3);
    plot(y0, 'r');
    hold on;
    plot(y0_filt, 'b');
    title('y0');
    legend('Original', 'Filtered');

subplot(2, 2, 4);
    plot(y1, 'r');
    hold on;
    plot(y1_filt, 'b');
    title('y1');
    legend('Original', 'Filtered');

% Fit data
M0 = fit([x0_filt, x1_filt], y0_filt, 'poly33', 'Robust', 'LAR');
M1 = fit([x0_filt, x1_filt], y1_filt, 'poly33', 'Robust', 'LAR');

y0_fit = M0([x0, x1]);
y1_fit = M1([x0, x1]);

% Plot original data vs fitted
figure;
subplot(2, 1, 1);
    plot(y0, 'r');
    hold on;
    plot(y0_fit, 'b');
    title('y0');
    legend('Original', 'Fitted');

subplot(2, 1, 2);
    plot(y1, 'r');
    hold on;
    plot(y1_fit, 'b');
    title('y1');
    legend('Original', 'Fitted');

% Reconstruct theta and phi
theta_fit = sqrt(y0_fit.^2 + y1_fit.^2);
phi_fit = atan2(y1_fit, y0_fit);

% Plot original vs reconstructed
figure;
subplot(2, 1, 1);
    plot(theta, 'r');
    hold on;
    plot(theta_fit, 'b');
    title('theta');
    legend('Original', 'Reconstructed');

subplot(2, 1, 2);
    plot(phi, 'r');
    hold on;
    plot(phi_fit, 'b');
    title('phi');
    legend('Original', 'Reconstructed');

% Calculate error
error_theta = theta - theta_fit;
error_phi = phi - phi_fit;

% Plot error
figure;
subplot(2, 1, 1);
    plot(error_theta * 180/pi);
    title('Error theta');

subplot(2, 1, 2);
    plot(error_phi * 180/pi);
    title('Error phi');

% Validate model with new data
theta_validation = data_validation{:, 1} * pi / 180;
phi_validation = data_validation{:, 2} * pi / 180;
for i = 1:length(phi_validation)
    if theta_validation(i) < 0
        theta_validation(i) = -theta_validation(i);
        phi_validation(i) = phi_validation(i) - pi;
    end

    if phi_validation(i) < 0
        phi_validation(i) = phi_validation(i) + 2*pi;
    end
end

x0_validation = data_validation{:, 3};
x1_validation = data_validation{:, 4};

y0_validation = theta_validation .* cos(phi_validation);
y1_validation = theta_validation .* sin(phi_validation);

y0_fit_validation = M0([x0_validation, x1_validation]);
y1_fit_validation = M1([x0_validation, x1_validation]);

% Plot validation data
figure;
subplot(2, 1, 1);
    plot(y0_validation, 'r');
    hold on;
    plot(y0_fit_validation, 'b');
    title('y0');
    legend('Original', 'Fitted');

subplot(2, 1, 2);
    plot(y1_validation, 'r');
    hold on;
    plot(y1_fit_validation, 'b');
    title('y1');
    legend('Original', 'Fitted');

% Reconstruct theta and phi
theta_fit_validation = sqrt(y0_fit_validation.^2 + y1_fit_validation.^2);
phi_fit_validation = atan2(y1_fit_validation, y0_fit_validation);
for i = 1:length(phi_fit_validation)
    if theta_fit_validation(i) < 0
        theta_fit_validation(i) = -theta_fit_validation(i);
        phi_fit_validation(i) = phi_fit_validation(i) - pi;
    end

    if phi_fit_validation(i) < 0
        phi_fit_validation(i) = phi_fit_validation(i) + 2*pi;
    end
end

% Plot original vs reconstructed
figure;
subplot(2, 1, 1);
    plot(theta_validation * 180/pi, 'r');
    hold on;
    plot(theta_fit_validation * 180/pi, 'b');
    title('theta');
    legend('Original', 'Reconstructed');

subplot(2, 1, 2);
    plot(phi_validation * 180/pi, 'r');
    hold on;
    plot(phi_fit_validation * 180/pi, 'b');
    title('phi');
    legend('Original', 'Reconstructed');

% 3D plot: [theta0, theta1, sample_index]
figure;
    plot3(y0_validation, y1_validation, 1:length(theta_validation), 'r');
    hold on;
    plot3(y0_fit_validation, y1_fit_validation, 1:length(theta_validation), 'b');
    title('3D plot');
    legend('Original', 'Reconstructed');

% Calculate error
error_theta = theta_validation - theta_fit_validation;
error_phi = phi_validation - phi_fit_validation;

% Plot error
figure;
subplot(2, 1, 1);
    plot(error_theta * 180/pi);
    title('Error theta');

subplot(2, 1, 2);
    plot(error_phi * 180/pi);
    title('Error phi');

% Save model
save(modelname, 'M0', 'M1');