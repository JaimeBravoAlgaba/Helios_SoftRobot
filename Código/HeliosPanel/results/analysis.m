file = "31032023_2.mat";
load(file);
load("net.mat");

predictors = test.data;
responses = test.ang;

prediction = net(predictors')';

figure; set(gcf, 'Color', 'w');
subplot(311);
    grid on; hold on; title("Sensor readings");
    plot(test.data);
    xlim([0,376]);
    ylim([0,4e4]);

subplot(312);
    grid on; hold on; title("Steps per axis");
    plot(test.ang);
    xlim([0,376]);
    ylim([-4e3,4e3]);

subplot(313);
    grid on; hold on; title("Prediction");
    plot(prediction);
    xlim([0,376]);
    ylim([-4e3,4e3]);