function [part1,part2] = bisection_spectral(A,xy,picture)
% bisection_spectral : Spectral partition of a graph.
%
% D.P & O.S for the "HPC Course" at USI and
%                   "HPC Lab for CSE" at ETH Zuric
%
% [part1,part2] = bisection_spectral(A) returns a partition of the n vertices
%                 of A into two lists part1
% and part2 according to the
%                 spectral bisection algorithm of Simon et al:
%                 Label the vertices with the components of the Fiedler vector
%                 (the second eigenvector of the Laplacian matrix) and partition
%                 them around the median value or 0.
    
    % Steps
    % 1. Construct the Laplacian.
    L = diag(sum(A,2)) - A;
    
    % 2. Calculate its eigensdecomposition.
    [Vecs,~] = eigs(L, 2, 'smallestreal');
    FiedlerVec = Vecs(:,2);
    threshold = median(FiedlerVec);
    
    % 3. Label the vertices with the components of the Fiedler vector.
    n = size(A,1);
    map = zeros(n,1);
    map(FiedlerVec >= threshold) = 0;
    map(FiedlerVec < threshold) = 1;
    [part1, part2] = other(map);
    
    % 4. Partition them around their median value, or 0.
    if picture == 1
        gplotpart(A,xy,part1);
        title('Spectral bisection using the Fiedler Eigenvector');
    end

end
