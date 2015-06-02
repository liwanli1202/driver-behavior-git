
kalman_data

xl = [data.dPerpLeft];
yl = [data.dYawLeft];
ll = [data.likelihoodLeft];
xr = [data.dPerpRight];
yr = [data.dYawRight];
lr = [data.likelihoodRight];

fp = fopen('octave-out.txt', 'r');
pos = fscanf(fp, '%f');
fclose(fp);

fp = fopen('data.cpp','w');

fprintf(fp, 'static double dPerpLeftSeq3[] = { %f', xl(1));
fprintf(fp, ', %f', xl(2:end));
fprintf(fp, ' };\n');
fprintf(fp, 'static double dPerpRightSeq3[] = { %f', xr(1));
fprintf(fp, ', %f', xr(2:end));
fprintf(fp, ' };\n');
fprintf(fp, 'static double dYawLeftSeq3[] = { %f', yl(1));
fprintf(fp, ', %f', yl(2:end));
fprintf(fp, ' };\n');
fprintf(fp, 'static double dYawRightSeq3[] = { %f', yr(1));
fprintf(fp, ', %f', yr(2:end));
fprintf(fp, ' };\n');
fprintf(fp, 'static double dLikelihoodLeftSeq3[] = { %f', ll(1));
fprintf(fp, ', %f', ll(2:end));
fprintf(fp, ' };\n');
fprintf(fp, 'static double dLikelihoodRightSeq3[] = { %f', lr(1));
fprintf(fp, ', %f', lr(2:end));
fprintf(fp, ' };\n');
fprintf(fp, 'static double dPositionFromOctaveSeq3[] = { %f', pos(1));
fprintf(fp, ', %f', pos(2:end));
fprintf(fp, ' };\n');

fclose(fp);

