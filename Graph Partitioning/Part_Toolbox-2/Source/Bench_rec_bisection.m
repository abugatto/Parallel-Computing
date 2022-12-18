% Benchmark for recursively partitioning meshes, based on various
% bisection approaches
%
% D.P & O.S for the "HPC Course" at USI and
%                   "HPC Lab for CSE" at ETH Zurich



% add necessary paths
addpaths_GP;
nlevels_a = 3;
nlevels_b = 4;

fprintf('       *********************************************\n')
fprintf('       ***  Recursive graph bisection benchmark  ***\n');
fprintf('       *********************************************\n')

% load cases
cases = {
    'mesh1e1.mat';
    'mesh2e1.mat';
    'mesh3e1.mat';
    'mesh3em5.mat';
    'airfoil1.mat';
    'netz4504_dual.mat';
    'stufe.mat';
    '3elt.mat';
    'barth4.mat';
    'ukerbe1.mat';
    'crack.mat';
    };


nc = length(cases);
maxlen = 0;
for c = 1:nc
    if length(cases{c}) > maxlen
        maxlen = length(cases{c});
    end
end

for c = 1:nc
    fprintf('.');
    sparse_matrices(c) = load(cases{c});
end


fprintf('\n\n Report Cases         Nodes     Edges\n');
fprintf(repmat('-', 1, 40));
fprintf('\n');
for c = 1:nc
    spacers  = repmat('.', 1, maxlen+3-length(cases{c}));
    [params] = Initialize_case(sparse_matrices(c));
    fprintf('%s %s %10d %10d\n', cases{c}, spacers,params.numberOfVertices,params.numberOfEdges);
end

%% Create results table
fprintf('\n%7s %16s %20s %16s %16s\n','Bisection','Spectral','Metis 5.0.2','Coordinate','Inertial');
fprintf('%10s %10d %6d %10d %6d %10d %6d %10d %6d\n','Partitions',8,16,8,16,8,16,8,16);
fprintf(repmat('-', 1, 100));
fprintf('\n');

for c = 1:nc
    spacers = repmat('.', 1, maxlen+3-length(cases{c}));
    fprintf('%s %s', cases{c}, spacers);
    sparse_matrix = load(cases{c});
    

    % Recursively bisect the loaded graphs in 8 and 16 subgraphs.
    % Steps
    % 1. Initialize the problem
    [params] = Initialize_case(sparse_matrices(c));
    W      = params.Adj;
    coords = params.coords;

    % 2. Recursive routines
    % i. Spectral    
    [mapS8, sepijS8, sepAS8] = rec_bisection(@bisection_spectral, nlevels_a, W, coords, 0);
    [mapSF, sepijSF, sepASF] = rec_bisection(@bisection_spectral, nlevels_b, W, coords, 0);

    % ii. Metis
    [mapM8, sepijM8, sepAM8] = rec_bisection(@bisection_metis, nlevels_a, W, coords, 0);
    [mapMF, sepijMF, sepAMF] = rec_bisection(@bisection_metis, nlevels_b, W, coords, 0);

    % iii. Coordinate    
    [mapC8, sepijC8, sepAC8] = rec_bisection(@bisection_coordinate, nlevels_a, W, coords, 0);
    [mapCF, sepijCF, sepACF] = rec_bisection(@bisection_coordinate, nlevels_b, W, coords, 0);

    % iv. Inertial
    [mapI8, sepijI8, sepAI8] = rec_bisection(@bisection_inertial, nlevels_a, W, coords, 0);
    [mapIF, sepijIF, sepAIF] = rec_bisection(@bisection_inertial, nlevels_b, W, coords, 0);

    % 3. Calculate number of cut edges
    [numS8, ~] = cutsize(W, mapS8);
    [numSF, ~] = cutsize(W, mapSF);

    [numM8, ~] = cutsize(W, mapM8);
    [numMF, ~] = cutsize(W, mapMF);

    [numC8, ~] = cutsize(W, mapC8);
    [numCF, ~] = cutsize(W, mapCF);

    [numI8, ~] = cutsize(W, mapI8);
    [numIF, ~] = cutsize(W, mapIF);

    % 4. Visualize the partitioning result
    fprintf('%s %s %6d %6d %10d %6d %10d %6d %10d %6d\n', ...
        cases{c}, spacers, numS8, numSF, numM8, numMF, numC8, numCF, numI8, numIF);

    if strcmp(cases{c}, 'crack.mat')
        root = '';

        gplotmap(W, coords, mapSF);
        set(gcf, 'InvertHardCopy', 'off'); 
        set(gcf,'Color',[0 0 0]);
        saveas(gcf, 'spectral16.png');

        gplotmap(W, coords, mapMF);
        set(gcf, 'InvertHardCopy', 'off'); 
        set(gcf,'Color',[0 0 0]);
        saveas(gcf, 'metis16.png');

        gplotmap(W, coords, mapCF);
        set(gcf, 'InvertHardCopy', 'off'); 
        set(gcf,'Color',[0 0 0]);
        saveas(gcf, 'coord16.png');

        gplotmap(W, coords, mapIF);
        set(gcf, 'InvertHardCopy', 'off'); 
        set(gcf,'Color',[0 0 0]);
        saveas(gcf, 'inertial16.png');
    end
end
