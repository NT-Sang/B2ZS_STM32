      clear ;
      clc;
      close all;

        %%
        header_tx = [1 1 1 1 1 1 1 1];
        flm =8000;
        bitrate = 100;
        sample_per_bit =flm/bitrate; 

        B2Zs_H = [];
        count =1;
        i=1;
        while(i<=length(header_tx))
            if(header_tx(i) ==1)
                if(count ==1)
                    B2Zs_H =[B2Zs_H 1];
                    count =0;
                else
                    B2Zs_H = [B2Zs_H -1];
                    count = 1;
                end
            elseif (i<length(header_tx) && header_tx(i)==0 && header_tx(i+1) ==0)
                if(count ==1)
                     B2Zs_H = [B2Zs_H 1 1];
                     count=0;
                else
                    B2Zs_H = [B2Zs_H -1 -1];
                    count =1;
                end
                i=i+1;
            else
                B2Zs_H = [B2Zs_H 0];
            end
             i =i+1;
        end
        B2Zs_H = B2Zs_H';
        B2Zs_H= [zeros(length(B2Zs_H),1) B2Zs_H]';
        B2Zs_H = B2Zs_H(:);
        B2Zs_H = kron(ones(1, sample_per_bit/2), B2Zs_H)';
        header = (B2Zs_H(:));


%
r = audiorecorder(8000,16,1);
record(r);
%
pause(15);

  
    y = getaudiodata(r, 'double');
    %
    subplot(4,1,1);
    plot(y);
    title('Tín hiệu thu được');


    %
[corr, lags] = xcorr(y,header );
[maxCorr, maxLagIndex] = max(corr);
timeLag = lags(maxLagIndex);
subplot(4,1,2);
plot(lags, corr);
title('Tín hiệu lấy tương quan');
%
y_a = y(timeLag:end);
subplot(4,1,3);
plot(y_a);


%
ys = y_a(60:80:end);
subplot(4,1,4);
stem(ys);

%
yc=zeros(size(ys));
yc(ys>0.5)=1;
yc(ys<-0.5)=-1;
yc=yc';
size_yc = length(yc);
ms = zeros(1, size_yc);
    i = 1;
    while i <= size_yc
        if yc(i) == 0
            ms(i) = 0; 
        elseif i < size_yc && ((yc(i) == 1 && yc(i+1) == 1) || (yc(i) == -1 && yc(i+1) == -1))
            ms(i) = 0;
            ms(i+1) = 0; 
            i = i + 1; 
        else
            ms(i) = 1; 
        end
        i = i + 1;
    end
    y = ms;
%
a = length(header_tx);
%
dodai = ms(length(header_tx)+1:length(header_tx)*3 );
%
dodai1 = bin2dec(strrep(num2str(dodai), ' ', ''));
%
data_tx = ms(a*3+1: a*3 + dodai1 );

%
%
% chuyen thanh text

a = [];
for i=1:8:length(data_tx)
    y_b = data_tx(i:i+7);
    y_c = bin2dec(strrep(num2str(y_b), ' ', ''));
    y_d=char(y_c);
    a=[a y_d];
end
a