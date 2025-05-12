%% ECE 4950 Spring 2025 - Golf Board Image Processing

%% Set up camera
cam = webcam('Brio 100');  % Connect to webcam

% Initialize golf object structure
golf.locBall = [0, 0; 0, 0];       % Max 2 balls
golf.locHole = [0, 0; 0, 0; 0, 0]; % Max 3 holes
golf.numBall = 0;
golf.numHole = 0;


%% Define hardcoded points (top-left, top-right, bottom-right, bottom-left)
inputPoints = [
    8.2450  152.6606
  637.8554  139.6263
  638.9888  454.1482
   15.0455  464.3489
];

%% Define output 
outputPoints = [
    0   0;
    700 0;
    700 350;
    0   350
];

%% Compute transformation
tform = fitgeotrans(inputPoints, outputPoints, 'projective');

%% Ask FIRST if user wants to set new background
answer1 = questdlg('Do you want to take a new background photo?', ...
    'New Background?', 'Yes','No','No');

switch answer1
    case 'Yes'
        % Take background image and apply warp
        rawBG = snapshot(cam);
        warpedBG = imwarp(rawBG, tform);
        
        %figure('Name', 'Warped Background');  % Create a new figure window
        %imshow(warpedBG);                     % Show the warped image
        %title('Click TOP-LEFT and BOTTOM-RIGHT corners of the putting green');

         %User selects two corners
        %[x1, y1] = ginput(1);  % Top-left
        %[x2, y2] = ginput(1);  % Bottom-right

        % Build crop rectangle
        %cropRect = [x1, y1, x2 - x1, y2 - y1];
        %disp('Use this cropRect for future runs:');
        %disp(cropRect);

        % Crop and save background
        cropRect = [17.2425, 170.3232, 700.1647, 349.1336];
        golf.background = imcrop(warpedBG, cropRect);
        imwrite(golf.background, 'background.png');
        save('cropRect.mat', 'cropRect');  % Save crop rectangle for reuse

    case 'No'
        golf.background = imread('background.png');
        load('cropRect.mat', 'cropRect');  % Load crop rectangle
end

%% Capture and process current image
waitfor(msgbox("Ready to take current photo?"));
raw = snapshot(cam);
warped = imwarp(raw, tform);

% Show warped image
%figure('Name', 'Warped Image - Current');
%imshow(warped);
%title('Warped Image - Before Cropping');

% Crop to putting green
golf.current = imcrop(warped, cropRect);
imwrite(golf.current, 'current.png');
clear cam;

%% Background Subtraction
golf.sub = golf.current - golf.background;

%% Highlight motion/differences
[height, width, ~] = size(golf.current);
sub2 = golf.sub;

for i = 1:height
    for j = 1:width
        if (golf.sub(i,j,1) > 100) || ...
           (golf.sub(i,j,2) > 100) || ...
           (golf.sub(i,j,3) > 100)
            sub2(i,j,:) = [175, 200, 175];  % Mark as green
        end
    end
end

binary = im2bw(sub2);

%% Morphological filtering
SE = strel('disk', 5);
eroded = imerode(binary, SE);

SE = strel('disk', 7);
golf.subNoNoise = imdilate(eroded, SE);

%% Get region properties
golf.stats = regionprops(golf.subNoNoise, 'all');

