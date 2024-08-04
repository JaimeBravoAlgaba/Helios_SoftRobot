a = [0, 30, 45, 60, 90];
clad = [126.9278, 97.4177, 86.1797, 76.6126, 45.3909];
core = [241.5301, 181.4205, 170.0201, 151.1259, 94.1238];

figure;
set(gcf, 'Color', 'w');
plot(a, clad, '-b', 'LineWidth', 1.5, 'Marker', '.', 'MarkerSize', 30);
hold on;
plot(a, core, '-r', 'LineWidth', 1.5, 'Marker', '.', 'MarkerSize', 30);
set(gca, 'FontSize', 14);

xlabel('Ángulo doblado [grados]', 'FontSize', 14);
ylabel('Luminancia media', 'FontSize', 14);
legend('Cobertura', 'Núcleo', 'FontSize', 14);
grid on;