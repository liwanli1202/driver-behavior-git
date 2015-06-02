function make_sim_image(P, leftBoundaryDist, rightBoundaryDist, laneMarkingWidth, yaw)

if nargin < 5
  yaw = 0;
end

if nargin < 4
  laneMarkingWidth = 0.10;
end

if nargin < 3
  rightBoundaryDist = 1.015;
end

if nargin < 2
  leftBoundaryDist = -1.222;
end

if nargin < 1
  P = [ 7.6844e+02   3.0425e+02  -1.5473e+02   2.5834e+02
       -5.9223e+01  -1.1659e+01  -8.1157e+02   1.3550e+03
       -2.2755e-02   9.5140e-01  -3.0711e-01   5.1275e-01 ];
end

% Camera intrinsics

nRows = 480;
nCols = 640;

[K, R, C] = decomposecamera(P);

simImage = getSimImage(nCols, nRows, P, C, leftBoundaryDist, rightBoundaryDist, laneMarkingWidth);

figure(1);
imshow(simImage);
title('Simulated road image');

imwrite(simImage, "simImage.png");
fid = fopen("simImage.params","w");
fprintf(fid, '%%YAML:1.0\n');
fprintf(fid, 'hasProjectionMatrixParams: 1\n');
fprintf(fid, 'ProjectionMatrix: !!opencv-matrix\n');
fprintf(fid, '   rows: 3\n   cols: 4\n   dt: d\n   data: [');
Pvec = P'(:);
fprintf(fid, ' %f,', Pvec(1:end-1));
fprintf(fid, ' %f ]\n', Pvec(end));
fprintf(fid, 'distLeftGroundTruth: %f\n', abs(leftBoundaryDist));
fprintf(fid, 'distRightGroundTruth: %f\n', abs(rightBoundaryDist));
fprintf(fid, 'iCameraWidth: %d\n', nCols);
fprintf(fid, 'iCameraHeight: %d\n', nRows);
fclose(fid);

end

%------------------------------------------------------------------------------

function I = getSimImage(nC, nR, P, camCtr, leftBound, rightBound, boundaryWidth)

if nargin ~= 7
  error('I = getSimImage(nC, nR, P, camCtr, leftBound, rightBound, boundaryWidth');
end

I = uint8(zeros(nR, nC, 3));

Pinv = pinv(P);
for row = 1:nR
  for col = 1:nC
    x = [ col ; row ; 1 ];
    Ximg = Pinv * x;
    gndX = gndPlaneIntersect(Ximg(1:3) / Ximg(4), camCtr);
    if (gndX(2) < 0)
      % Sky
      I(row,col,:) = [0, 0, 255];
    else
      if abs(gndX(1)-leftBound) < boundaryWidth/2 || abs(gndX(1)-rightBound) < boundaryWidth/2
        I(row,col,:) = [255, 255, 255];
      else
        I(row,col,:) = [128, 128, 128];
      end
    end
  end
end

end

%------------------------------------------------------------------------------

function X = gndPlaneIntersect(X1, X2)

Zdiff = X2(3) - X1(3);
if abs(Zdiff) < 1e-3
  X = [ 0 ; -1e+4 ; 0 ];
else
  X = X1 - (X2-X1) * X1(3) / Zdiff;
end

end

%------------------------------------------------------------------------------
