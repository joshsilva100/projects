%% ECE 4951 Spring 2025 Project 2 - Image Processing 

%% Set up camera 
camName = 'Brio 101'; 
cam = webcam(camName); %Assign lab camera 
rect = [3.51000000000000, 68.5100000000000, 627.980000000000, 312.980000000000]; %Set up cropping

%% Set up game state structure measurements
golf.locBall = [0, 0; 0, 0]; %All of the locations for Golfballs (2 MAX) 
golf.locHole = [0, 0; 0, 0; 0, 0]; %All of the locations for Golfholes (3 MAX)
golf.numBall = 0; %Initialize to 0
golf.numHole = 0; 

%% Set up images. Take photo of background and of current. 
answer1 = questdlg('Do you want to take a new background photo?', ...
	'New Background?', ...
	'Yes','No','No');

%Handle user's answer to new background image or not 
switch answer1 
    case 'Yes'
        golf.background = imcrop(snapshot(cam), rect);
        imwrite(golf.background, "background.png"); %Save new background
    case 'No' 
        golf.background = imread('background.png'); %Use last background
end 

waitfor(msgbox("Ready to take current photo?"));
golf.current = imcrop(snapshot(cam), rect); %Take a live picture of background 
imwrite(golf.current, "current.png"); %Save new current
clear('cam'); %Don't need camera anymore 

%% Background Subtraction
golf.sub = golf.current - golf.background; %Take difference for background subtraction 

%% Fix Image to Compute Binary Image: Necessary to pick up less extreme colors
[height,width,depth] = size(golf.current);
sub2 = golf.sub; 

% This is RGB, 3 pixels per X,Y coordinate. 0,0,0 for RGB is black, while 
% 255,255,255 is white. See original sub image & detect if color is
% distinct enough from old background (grey/black).
for i=1:height
    for j=1:width 
        %Account for threshold.
        if (golf.sub(i,j,1) > 100) || ...
           (golf.sub(i,j,2) > 100) || ...
           (golf.sub(i,j,3) > 100)

            %Will Show in Green
            sub2(i,j,:) = [175,200,175];
        end
    end
end

binary = im2bw(sub2);

%% Erode Image: 
%Set Morphological Operation
SE = strel('disk',5);

erode = imerode(binary, SE);

%% Dilate Image: Golfballs tend to not be picked up well
% Set Morphological Operation
SE = strel('disk',7);

golf.subNoNoise = imdilate(erode, SE); %Store image with noise removal

%% Call regionprops: Store data like centroids
golf.stats = regionprops(golf.subNoNoise, 'all'); 
