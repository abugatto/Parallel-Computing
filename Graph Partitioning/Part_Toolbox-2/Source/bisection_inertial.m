%
% D.P & O.S for the "HPC Course" at USI and
%                   "HPC Lab for CSE" at ETH Zurich

function [part1,part2] = bisection_inertial(A,xy,picture)
% bisection_inertial : Inertial partition of a graph.
%
% [p1,p2] = bisection_inertial(A,xy) returns a list of the vertices on one side of a partition
%     obtained by bisection with a line or plane normal to a moment of inertia
%     of the vertices, considered as points in Euclidean space.
%     Input A is the adjacency matrix of the mesh (used only for the picture!);
%     each row of xy is the coordinates of a point in d-space.
%
% bisection_inertial(A,xy,1) also draws a picture.
    
    % Steps
    % 1. Calculate the center of mass.
    COM = mean(xy);
    
    % 2. Construct the matrix M.
    %  (Consult the pdf of the assignment for the creation of M) 
    normalized = xy - COM;
    Sxx = sum(power(normalized(:,1), 2));
    Sxy = sum(normalized(:,1) .* normalized(:,2));
    Syy = sum(power(normalized(:,2), 2));
    M = [Syy, Sxy; Sxy, Sxx];
    
    % 3. Calculate the smallest eigenvector of M.  
    [Vec,~] = eigs(M, 1,'smallestabs');
    
    % 4. Find the line L on which the center of mass lies.
    L = Vec / sum(Vec); % normalized Vec from paper
    
    % 5. Partition the points around the line L.
    %   (you may use the function partition.m)
    [part1, part2] = partition(xy, L);
    
    if picture == 1
        gplotpart(A,xy,part1);
        title('Inertial bisection using the Fiedler Eigenvector');
    end
end
