% RANGE ANALASYS
addpath('../dataset/240510');
module = '0x44';
dataset = '_240510_';

h0min = 0;
h1min = 0;

h0max = 0;
h1max = 0;

%% DATA EXTRACTION AND PREPROCESSING
N = 3;
for i = 1:N
    data = readtable(strcat(module, dataset, string(i), '.csv'));
    h0 = data{:, 4};
    h1 = data{:, 5};

    h0min = h0min + min(h0)/N;
    h1min = h1min + min(h1)/N;

    h0max = h0max + max(h0)/N;
    h1max = h1max + max(h1)/N;
end

save(strcat('minmax_', module, dataset(1:end-1) , '.mat'), 'h0min', 'h0max', 'h1min', 'h1max');