A=load('ROC2B.txt');
B=load('ROC2B2.txt');
plot(A(:,2),A(:,1))
hold on
plot(B(:,2),B(:,1))
hold off
