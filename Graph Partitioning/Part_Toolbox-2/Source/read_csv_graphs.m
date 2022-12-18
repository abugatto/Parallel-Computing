% Script to load .csv lists of adjacency matrices and the corresponding 
% coordinates. 
% The resulting graphs should be visualized and saved in a .csv file.
%
% D.P & O.S for the "HPC Course" at USI and
%                   "HPC Lab for CSE" at ETH Zurich

addpaths_GP;

csvpath = strcat(pwd, '/Part_Toolbox-2/Datasets/Countries_Meshes/csv/');
matpath = strcat(pwd, '/Part_Toolbox-2/Datasets/Countries_Meshes/mat/');
root = '';

files = dir(strcat(csvpath, '*-adj.csv'));

for i = 1:size(files)
    suffix = '-adj.csv';
    name = files(i).name;
    name = name(1:length(name)-length(suffix));

    % Steps
    % 1. Load the .csv files
    Adj = csvread(strcat(csvpath, name, '-adj.csv'), 1, 0);
    C = csvread(strcat(csvpath, name, '-pts.csv'), 1, 0);
    % 2. Construct the adjaceny matrix (NxN). There are multiple ways
    %    to do so.
    W = sparse(cat(1, Adj(:,1), Adj(:,2)), cat(1, Adj(:,2), Adj(:,1)), ones(2*size(Adj, 1), 1), length(C), length(C));
    W = spfun(@(x) x ~= 0, W);
    % 4. Save the resulting graphs
    save(strcat(matpath, name, '.mat'), 'W', "C");
end

% 3. Visualize the resulting graphs
figure;

load NO-9935.mat;
subplot(2,1,1);
gplotg(W,C);
title('Norway');

load VN-4031.mat;
subplot(2,1,2);
gplotg(W, C);
title('Vietnam');

saveas(gcf, strcat(root, 'connectivity.png'))

