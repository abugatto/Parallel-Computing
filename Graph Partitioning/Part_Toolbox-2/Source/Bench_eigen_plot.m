% Visualize information from the eigenspectrum of the graph Laplacian
%
% D.P & O.S for the "HPC Course" at USI and
%                   "HPC Lab for CSE" at ETH Zurich

% add necessary paths
addpaths_GP;

% Graphical output at bisection level
picture = 0;

% Cases under consideration
cases = {
    'airfoil1.mat';
    'mesh3e1.mat';
    '3elt.mat';
    'barth4.mat';
    'crack.mat';
    };

% Steps
for c = 1:length(cases)
    % Initialize the cases
    load(cases{c});
    W      = Problem.A;
    coords = Problem.aux.coord;

    % 1. Construct the graph Laplacian of the graph in question.
    W = spfun(@(x) x ~= 0, W);
    L = diag(sum(W,2)) - W;
    
    % 2. Compute eigenvectors associated with the smallest eigenvalues.
    [Vecs,~] = eigs(L, 3, 'smallestreal');
    FiedlerVec2 = Vecs(:,2);
    FiedlerVec3 = Vecs(:,3);
    threshold = median(FiedlerVec2);
    
    % 3. Perform spectral bisection.
    n = size(W,1);
    map = zeros(n,1);
    map(FiedlerVec2 >= threshold) = 0;
    map(FiedlerVec2 < threshold) = 1;
    [part1, part2] = other(map);
    
    % 4. Visualize:
    %   i.   The first and second eigenvectors.
    gplotpart(W, [FiedlerVec2 FiedlerVec3], part1);
    title('Partition with spectral coordinates');
    xlabel('v2');
    ylabel('v3');
    set(gcf, 'InvertHardCopy', 'off'); 
    set(gcf,'Color',[0 0 0]);
    saveas(gcf, strcat(cases{c}, '_with_spectral_coords.png'));
end



