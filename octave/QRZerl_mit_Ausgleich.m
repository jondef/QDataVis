clc;
clear;
close all;

%% Ausgleichsrechnung ohne Anpassung der x-Achse

figure();
%data = readmatrix('data.txt'); % for matlab
data = dlmread('data1.txt', ' ', 0, 0);
f = data(:,1);  %x -> A
signal = data(:,2); %y -> b
plot(f, signal, 'b.') ;hold on;


m = length(f);       %Zeilen
n = 10;               %Spalten

%Koeffizient vor fz und Polynom mit Ausgleichsrechnung zu bestimmen
% (Ohne anpassung der X-Koordinate)
x1 = f;
x0 = ones(m,1).*80.3e3;
z1 = (x1-x0)./50;       %definiert Breite und Ort des Peaks
fz = 1./(1+(z1.^2));              %f = a*1/(1+x'^2), x'= (x-x0)/50

% Aufstellen von A
for degree = 1:n
    if degree == n
        A(1:m, degree) = ones(m,1);
    else
        A(1:m, degree) = f.^(n - degree);
    end
end

%A(1:m, 1) = f.^8;
%A(1:m, 2) = f.^7;
%A(1:m, 3) = f.^6;
%A(1:m, 4) = f.^5;
%A(1:m, 5) = f.^4;
%A(1:m, 6) = f.^3;
%A(1:m, 7) = f.^2;
%A(1:m, 8) = f.^1;
%A(1:m, 9) = ones(m,1);

%A(1:m, 10) = fz;

Im = eye(m);

% QR-Zerlegung von A
R = A;
Q = Im;
for k = 1:n
    x = R(k:m, k);
    if x(k:k) < 0
        lambda = norm(x);
    else
        lambda = -norm(x);
    end
    % sz = [m-k+1,1];
    e = Im(k:m,k);
    v = (x-lambda*e)/norm(x-lambda*e);
    
    z = zeros(k-1,1);
    
    H = eye(m)-2.*[z;v]*[z;v]';
    Q = Q*H;
    
    R = H*R;
    
end

clear x;

%Rückwärtseinsetzen
y = Q'*signal;
                   % R*x = y;
x(n) = y(n)/R(n,n);
for i= (n-1):-1:1
    s=0;
    for j=i+1:n
        s = s + x(j)*R(i,j); % x(2) = (y(2) - R23*x(3))/R22;
    end
    x(i) = (y(i) - s)/R(i,i);
end
x = x';

%Kondition der Matrix A 
Kondition_ohne_anpassen = cond(A);

fx = A*x;
%Display the function as text
y_sym = ''; exp(x)
for i=1:n
  y_sym = strcat(y_sym, ' + ', num2str(x(i)), 'x^', num2str(n-i));
end

disp(y_sym);

%plot(f, x(5).*fz, "r-");
plot(f, fx, 'm-', 'linewidth', 1);
title("Gefittete Kurve ohne x-Anpassung. Kond: "+ Kondition_ohne_anpassen);
legend("Messpünkte", "Peak alleine" ,"Polynom+Peak");
hold off;


%% Berechnen Ausgleichsrechnung mit Anpassung auf x-Achse (80k bis 81k --> 0 bis 1) auf x-achse
#{
figure();

%Koeffizient vor fz und Polynom mit Ausgleichsrechnung zu bestimmen
% (Ohne anpassung der X-Koordinate)
x1 = (f-80e3)./1000;
x0 = (ones(m,1).*(80.3e3 - 80e3))./1000;
z1 = (x1-x0)./50;       %definiert Breite und Ort des Peaks
fz = 1./(1+((z1.*1000).^2));              %f = a*1/(1+x'^2), x'= (x-x0)/50

plot(x1, signal, 'r.'); hold on;

% Aufstellen von A 
A(1:m, 1) = x1.^3;
A(1:m, 2) = x1.^2;
A(1:m, 3) = x1;
A(1:m, 4) = ones(m,1);
A(1:m, 5) = fz;

% QR-Zerlegung von A
R = A;
Q = Im;
for k = 1:n
    x = R(k:m, k);
    if x(k:k) < 0
        lambda = norm(x);
    else
        lambda = -norm(x);
    end
    % sz = [m-k+1,1];
    e = Im(k:m,k);
    v = (x-lambda*e)/norm(x-lambda*e);
    
    z = zeros(k-1,1);
    
    H = eye(m)-2.*[z;v]*[z;v]';
    Q = Q*H;
    
    R = H*R;
    
end

clear x;

%Rückwärtseinsetzen
y = Q'*signal;
                   % R*x = y;
x(n) = y(n)/R(n,n);
for i= (n-1):-1:1
    s=0;
    for j=i+1:n
        s = s + x(j)*R(i,j); % x(2) = (y(2) - R23*x(3))/R22;
    end
    x(i) = (y(i) - s)/R(i,i);
end
x = x';

%Kondition der Matrix A 
Kondition_mit_anpassen = cond(A);

fx = A*x;
plot(x1, x(5).*fz, "b-");
plot(x1, fx, 'c-');
title("Gefittete Kurve mit x-Anpassung, Kond: " + Kondition_mit_anpassen);
legend("Messpünkte", "Peak alleine" ,"Polynom+Peak");
hold off;
#}
   