function kalman

kalman_data

  N = size(data,2);
  
  leftDist = zeros(N,1);
  leftDistUsed = zeros(N,1);
  leftDistUsed(:) = nan;
  rightDist = zeros(N,1);
  rightDistUsed = zeros(N,1);
  rightDistUsed(:) = nan;
  leftYaw = zeros(N,1);
  righYaw = zeros(N,1);

  for i = 1:N
    if data(i).likelihoodLeft > 0
      leftDist(i) = data(i).dPerpLeft;
      leftYaw(i) = data(i).dYawLeft;
    else
      leftDist(i) = nan;
      leftYaw(i) = nan;
    end
  
    if data(i).likelihoodRight > 0
      rightDist(i) = data(i).dPerpRight;
      rightYaw(i) = data(i).dYawRight;
    else
      rightDist(i) = nan;
      rightYaw(i) = nan;
    end

  end
  
  figure(1);
  hold off;
  plot(-leftDist, 'r-');
  hold on;
  title('Vehicle position estimation results');
  ylabel('Lateral position');
  plot(rightDist, 'r-;Raw boundary measurements;');
  
  xposition = zeros(N,1);

  nearestPosition = zeros(N,1);
  zNearestPosition = zeros(N,1);
  numUpdatesNearest = 0;

  z = [ leftDist(1) ; leftYaw(1) ; rightDist(1) ; rightYaw(1) ];
  likelihoods = [ data(1).likelihoodLeft; data(1).likelihoodRight ];

  [xhat, Phat] = initializeState(z);
  %fprintf(1, 'xhat: %f %f %f %f\n', xhat);
  fprintf(1, '1: Position %f\n', xhat(1));

  F = [ 1 0   0 0 ;
        0 1   0 0 ;
        0 0   1 0 ;
        0 0   0 1 ];
  
  yawDrift = (2.5/180*pi)^2;
  Q = [ 0.01^2 0     0        0 ;
        0      0.1^2 0        0 ;
        0      0     yawDrift 0 ;
        0      0     0        0.1^2 ];
  
  H = [  1.0 0  0  0.5 ;
         0   0 -1  0   ;
        -1.0 0  0  0.5 ;
         0   0 -1  0   ];
  
  Hc = [ 0 ; pi/2 ; 0 ; pi/2 ];
  
  distMsmtErr = 0.5^2;
  yawMsmtErr = (5/180*pi)^2;

  Rfull = diag([distMsmtErr, yawMsmtErr, distMsmtErr, yawMsmtErr]);
  zorig = [ leftDist(1) ; leftYaw(1) ; rightDist(1) ; rightYaw(1) ];

  zNearest = zorig(1:2);
  if rightDist(1) < leftDist(1)
    zNearest = [ zorig(3:4) ];
  end
  xhatNearest = zNearest;
  Fnearest = eye(2);
  Qnearest = diag([0.10^2, (1/180*pi)^2]);
  Hnearest = eye(2);
  PhatNearest = Rnearest = Rfull(1:2,1:2);
  
  xposition(1) = xhat(1);
  nearestPosition(1) = xhatNearest(1);

  for i = 2:N

    R = Rfull;
    zLast = z;
    zNearestLast = zNearest;
    RnearestLast = Rnearest;
    z = [ leftDist(i) ; leftYaw(i) ; rightDist(i) ; rightYaw(i) ];
    likelihoodsLast = likelihoods;
    likelihoods = [ data(i).likelihoodLeft; data(i).likelihoodRight ];
    if isnan(z(1))
      z(1) = 0;
      z(2) = 0;
      R(1,1) = 1e+10;
      R(2,2) = 1e+10;
    end
    if isnan(z(3))
      z(3) = 0;
      z(4) = 0;
      R(3,3) = 1e+10;
      R(4,4) = 1e+10;
    end
    [z,R] = remove_bad_boundaries(z, R, expectedLaneWidth, likelihoods);
    if R(1,1) < 1e+10
      leftDistUsed(i) = z(1);
    end
    if R(3,3) < 1e+10
      rightDistUsed(i) = z(3);
    end

    debug = false;
    if i == 1944
      %debug = true;
    end

    if debug
      fprintf(1, 'xhat before: %f %f %f %f\n', xhat);
      fprintf(1, 'z: %f %f %f %f\n', z);
      R
    end

    laneChange = false;
    if abs(xhat(1)) < 0.2
      % No tracking of nearest lane boundary
      xhatNearest = [ nan ; nan ];
      Phatnearest = eye(2) * 1e+10;
      numUpdatesNearest = 0;
    else
      zNearest = [0 ; 0];
      Rnearest = eye(2) * 1e+10;
      if R(1,1) < 1e+5 && (R(3,3) > 1e+5 || z(1) < z(3))
        zNearest = [-z(1) ; z(2) ];
        Rnearest = R(1:2,1:2);
      elseif R(3,3) < 1e+5 && (R(1,1) > 1e+5 || z(3) < z(1))
        zNearest = z(3:4);
        Rnearest = R(3:4,3:4);
      end

      if ~isnan(xhatNearest(1)) || Rnearest(1,1) < 1e+5

        if isnan(xhatNearest(1))
          xhatNearest = zNearest;
          Phatnearest = Rnearest;
          numUpdatesNearest = 0;
        end

        numUpdatesNearest = numUpdatesNearest + 1;

        if Rnearest(1,1) < 1e+5 && RnearestLast(1,1) < 1e+5 && abs(zNearest(1) - zNearestLast(1)) > 1.0 && numUpdatesNearest < 4
          xhatNearest = zNearest;
          Phatnearest = Rnearest;
          nearestPosition(i-numUpdatesNearest+1:i-1) = nan;
          numUpdatesNearest = 1;
        else
        end

        if debug
          fprintf(1, 'xhatNearest before: %f %f\n', xhatNearest);
          fprintf(1, 'zNearest: %f %f\n', zNearest);
        end

        [xhatNearest, PhatNearest, possibleLaneChange, zNearest] = kal_nearest(
                xhatNearest, PhatNearest, zNearest,
                Fnearest, Qnearest, Hnearest, Rnearest, expectedLaneWidth);
        zNearestPosition(i) = zNearest(1);

        if debug
          fprintf(1, 'xhatNearest after: %f %f\n', xhatNearest);
        end

        if possibleLaneChange
          xposition(i) = xhat(1);
          laneChange = consistentLaneChange(xposition, nearestPosition,
               zNearestPosition, i);
          if laneChange
            [xhat, Phat] = initializeState(z, R, expectedLaneWidth);
          end
        end
      end
    end

    if debug
      fprintf(1,'Lane change = %d\n', laneChange);
    end

    if ~laneChange
      [xhat, Phat] = m_est(xhat, Phat, z, F, Q, H, Hc, R, debug);
    end

    if debug || 1
      %fprintf(1, 'xhat: %f %f %f %f\n', xhat);
      %Phat
      fprintf(1, '%d: Position %f\n', i, xhat(1));
    end

    xposition(i) = xhat(1);

    nearestPosition(i) = xhatNearest(1);

    if i == 7 && 0
      break
    end
  
  end
  
  plot(-leftDistUsed, 'r-;Used boundary measurements;', 'linewidth', 3);
  plot(rightDistUsed, 'r-', 'linewidth', 3);
  plot(xposition, 'b-;Estimated vehicle position;', 'linewidth', 2);
  plot(nearestPosition, 'k-;Nearest lane boudary position;', 'linewidth', 2);
  hold off;

end

%----------------------------------------------------------------------------

function [xhat, Phat] = m_est(xhat, Phat, z, F, Q, H, Hc, R, debug)
% Robust Estimation with Unknown Noise Statistics
% Durovic and Kovacevic
% IEEE Trans. Auto. Control 44(6): 1292-1296

  %fprintf(1, 'Input to update xhat: %f %f %f %f\n', xhat);

  ns = size(xhat,1);
  nm = size(z,1);

  xpred = F * xhat;
  Ppred = F * Phat * F' + Q;

  SST = [ Ppred , zeros(ns, nm) ; zeros(nm, ns), R ];

  S = chol(SST)';
  Sinv = inv(S);
  X = Sinv * [ eye(ns) ; H ];
  Y = Sinv * [ xpred ; z-Hc ];

  Omega = calc_robust_weights(X, Y, xhat);

  Phat = inv(X' * Omega * X);
  xhat = Phat * X' * Omega * Y;

  if debug || 0
    fprintf(1, 'output xhat_rob: %f %f %f %f\n', xhat);
    Phat
  end
end

%----------------------------------------------------------------------------

function Omega = calc_robust_weights(X, Y, xhat_prev)

  resid = Y - X * xhat_prev;
  Psi = influence(resid, 1);
  Psi(resid==0) = 1;
  resid(resid==0) = 1;
  weights = Psi ./ resid;
  Omega = diag(weights);

end

%----------------------------------------------------------------------------

function Psi = influence(resids, delta)
% Derivative of Huber's score function
% rho(z) = delta * abs(z) - delta^2/2, |z| >= delta
%        = z^2/2,                      |z| <  delta

% For standard Kalman filter, just return the residuals themselves:
%  Psi = resids;

% For robust Kalman filter, return the dampened residuals

  Psi = resids;
  Psi(resids > delta) = delta;
  Psi(resids < -delta) = -delta;

end

%----------------------------------------------------------------------------

function [xhat, Phat] = kal(xhat, Phat, z, F, Q, H, Hc, R, debug)
  xpred = F * xhat;
  Ppred = F * Phat * F' + Q;
  zpred = H * xpred + Hc;
  zresid = z - zpred;
  S = H * Ppred * H' + R;
  K = Ppred * H' * inv(S);
  xhat = xpred + K * zresid;
  Phat = (eye(4) - K * H) * Ppred;
end

%----------------------------------------------------------------------------

function bad = is_bad(z, expectedLaneWidth)

  bad = false;

  if z(1) > 1.1 * expectedLaneWidth || abs(z(2) - pi/2) > 20/180*pi
    bad = true;
    fprintf(1, 'Bad: %f %f\n', z);
  end

end

%----------------------------------------------------------------------------

function [z, R] = remove_bad_boundaries(z, R, expectedLaneWidth, likelihoods)

  % Rule out individually bad boundaries

  if is_bad(z(1:2), expectedLaneWidth)
    z(1) = z(2) = 0;
    R(1,1) = R(2,2) = 1e+10;
  end

  if is_bad(z(3:4), expectedLaneWidth)
    z(3) = z(4) = 0;
    R(3,3) = R(4,4) = 1e+10;
  end

  if R(1,1) > 1e+5 || R(3,3) > 1e+5
    % Already ruled out one or more boundaries
    return
  end

  yawDiff = abs(z(2) - z(4));
  laneWidth = z(1) + z(3);
  laneWidthStdev = expectedLaneWidth * 0.1;
  laneWidthZscore = abs((laneWidth - expectedLaneWidth) / laneWidthStdev);

  if yawDiff > 10/180*pi || laneWidthZscore > 2.0
    % Select boundary with higher likelihood and ignore the other
    if likelihoods(1) > likelihoods(2)
      z(3) = z(4) = 0;
      R(3,3) = R(4,4) = 1e+10;
    else
      z(1) = z(2) = 0;
      R(1,1) = R(2,2) = 1e+10;
    end
  end

end

%----------------------------------------------------------------------------

function [xhat, Phat] = kal_f(xhat, Phat, z, F, Q, H, Hc, R, expectedLaneWidth, likelihoods, debug)
  [z,R] = remove_bad_boundaries(z, R, expectedLaneWidth, likelihoods);
  [xhat, Phat] = kal(xhat, Phat, z, F, Q, H, Hc, R, debug);
end

%----------------------------------------------------------------------------

function [xhat, Phat] = initializeState(z, R, expectedLaneWidth)

  validLeft = true;
  validRight = true;
  if nargin > 1
    if R(1,1) > 1e+5
      validLeft = false;
    end
    if R(3,3) > 1e+5
      validRight = false;
    end
  end

  if validLeft && validRight
    xhat = [ (z(1) - z(3))/2 ;
             0 ;
             (pi/2-z(4) + pi/2-z(2))/2 ;
             z(3) + z(1) ];
  elseif validLeft
    zright = expectedLaneWidth - z(1);
    xhat = [ (z(1) - zright)/2 ;
             0 ;
             pi/2-z(2) ;
             expectedLaneWidth ];
  elseif validRight
    zleft = expectedLaneWidth - z(3);
    xhat = [ (zleft - z(3))/2 ;
             0 ;
             pi/2-z(4) ;
             expectedLaneWidth ];
  else
    xhat = [ 0 ; 0 ; pi/2 ; expectedLaneWidth ];
  end
  
  yawErr = (10/180*pi)^2;
  Phat = [ 1.0 0   0      0 ;
           0   1.0 0      0 ;
           0   0   yawErr 0 ;
           0   0   0      1 ];
end
%----------------------------------------------------------------------------

function [xhat, Phat, laneChange, z] = kal_nearest(
            xhat, Phat, z, F, Q, H, R, expectedLaneWidth)

  xposLast = xhat(1);

  laneChange = false;

  if isnan(xhat(1))
    return
  end

  if is_bad(z, expectedLaneWidth)
    z(:) = 0;
    R(1,1) = R(2,2) = 1e+10;
  end

  Hc = zeros(2,1);
  debug = false;
  [xhat, Phat] = m_est(xhat, Phat, z, F, Q, H, Hc, R, debug);

  if xposLast >= 0 && xhat(1) < 0 || xposLast <= 0 && xhat(1) > 0
    laneChange = true;
  end

  %fprintf(1, 'Nearest pos z %f\n', z(1));
  %fprintf(1, 'Nearest pos %f\n', xhat(1));

end

%----------------------------------------------------------------------------

function laneChange = consistentLaneChange(xpos, nearestpos, znearestpos, i)

  laneChange = false;

  N = 10;
  Ninliers = 7;

  if i < N
    return
  end

  xpos = xpos((i-N+1):i);
  nearestpos = nearestpos((i-N+1):i);
  znearestpos = znearestpos((i-N+1):i);

  nans = isnan(xpos) | isnan(nearestpos) | isnan(znearestpos);

  xpos = xpos(~nans);
  nearestpos = nearestpos(~nans);
  znearestpos = znearestpos(~nans);

  if length(xpos) < Ninliers
    return;
  end

  %fprintf(1, 'CORRELATIONS: %f %f %f\n', corrcoef(xpos, nearestpos),
  %    corrcoef(xpos, znearestpos), corrcoef(nearestpos, znearestpos));

  [nearestEstSlope,nnes] = ransacSlope(nearestpos);
  [znearestSlope,nzns] = ransacSlope(znearestpos);

  %fprintf(1, 'SLOPES: %f %f\n', nearestEstSlope, znearestSlope);
  %fprintf(1, 'INLIERS: %d %d\n', nnes, nzns);

  if nearestEstSlope < 0 && znearestSlope < 0 && nnes > 0.5 && nzns > 0.5
    [xposSlope,nxs] = ransacSlopeXpos(xpos,1);
    if xposSlope > 0 && nxs > 0.4
      laneChange = true;
    end
  elseif nearestEstSlope > 0 && znearestSlope > 0 && nnes > 0.5 && nzns > 0.5
    [xposSlope,nxs] = ransacSlopeXpos(xpos,-1);
    if xposSlope < 0 && nxs > 0.4
      laneChange = true;
    end
  end

end

%----------------------------------------------------------------------------

function [slope, inliers] = ransacSlope(ydata)

  if size(ydata,1) == 1
    Y = ydata';
  else
    Y = ydata;
  end;
  N = size(Y,1);
  X = [ (1:N)', ones(N,1) ];
  inliersBest = 0;
  lineBest = [ 0 0 0 ];
  slope = 0;
  inliers = 0;
  for i = 1:N-1
    for j = i+1:N
      A = - (Y(j)-Y(i));
      B = (X(j,1)-X(i,1));
      C = - (A * X(i,1) + B * Y(i));
      normFactor = norm([A B]);
      A = A / normFactor;
      B = B / normFactor;
      C = C / normFactor;
      inliersThis = 0;
      for k = 1:N
        dist = abs(A*X(k,1)+B*Y(k)+C);
        if dist < 0.1
          inliersThis = inliersThis + 1;
        end
      end
      if inliersThis > inliersBest
        inliersBest = inliersThis;
        lineBest = [ A B C ];
      end
    end
  end
  inliers = inliersBest/N;
  slope = -lineBest(1)/lineBest(2);
  %[ X, Y, X * [lineBest(1) ; lineBest(3)] - Y / lineBest(2) ]
end

%----------------------------------------------------------------------------

function [slope, inliers] = ransacSlopeXpos(ydata, dir)

  % Expecting a line increasing (dir > 0) or decreasing (dir < 0)
  % followed by a period where the position has been pulled in the
  % opposite direction.

  % Filter out the positions in the opposite direction before 
  % estimating the line and return the slope/number of inliers

  if size(ydata,1) == 1
    Y = ydata';
  else
    Y = ydata;
  end;
  Norig = size(Y,1);
  while size(Y,1) > 2 && (Y(end)-Y(end-1))*dir <= 0
    Y = Y(1:end-1);
  end
  N = size(Y,1);
  X = [ (1:N)', ones(N,1) ];
  inliersBest = 0;
  lineBest = [ 0 0 0 ];
  slope = 0;
  inliers = 0;
  for i = 1:N-1
    for j = i+1:N
      A = - (Y(j)-Y(i));
      B = (X(j,1)-X(i,1));
      C = - (A * X(i,1) + B * Y(i));
      normFactor = norm([A B]);
      A = A / normFactor;
      B = B / normFactor;
      C = C / normFactor;
      inliersThis = 0;
      for k = 1:N
        dist = abs(A*X(k,1)+B*Y(k)+C);
        if dist < 0.1
          inliersThis = inliersThis + 1;
        end
      end
      if inliersThis > inliersBest
        inliersBest = inliersThis;
        lineBest = [ A B C ];
      end
    end
  end
  inliers = inliersBest/N;
  slope = -lineBest(1)/lineBest(2);
  %[ X, Y, X * [lineBest(1) ; lineBest(3)] - Y / lineBest(2) ]
end

