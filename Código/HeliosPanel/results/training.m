file = "31032023_2.mat";
load(file);
load("net.mat");

predictors = test.data;
responses = test.ang;

prediction = net(predictors')';
