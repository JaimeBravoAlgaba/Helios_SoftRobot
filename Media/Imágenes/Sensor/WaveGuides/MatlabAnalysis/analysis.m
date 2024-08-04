filename = 'Cladding_0.png';
I = imread(filename);
G = rgb2gray(I);
disp(mean(mean(G)));
