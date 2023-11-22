function [profile, t, p] = stepperInterpolator(deltaPos, T, varargin)
    ip = inputParser;
    
    % Optinal arguments
    ip.addOptional('ts', 5e-6, @isnumeric);
    ip.addOptional('stepsPerRev', 7000, @isnumeric);
    
    ip.parse(varargin{:});
    
    ts = ip.Results.ts;
    stepsPerRev = ip.Results.stepsPerRev;

    stepAngle = 2*pi/stepsPerRev;
    n = ceil(T/ts);
    
    if(stepAngle<deltaPos/n)
        disp("Too fast profile, reduce speed.")
        profile = 0;
        t = 0;
    else
        v = deltaPos/T;    
        profile = zeros(1,n);    
        step_prev = 0;
        
        for i=1:n
            step = v*ts + step_prev;
            if(step>=stepAngle)
                profile(i) = 1;
                step_prev = step-stepAngle;
            else
                profile(i) = 0;
                step_prev = step;
            end
        end
        
        p = cumsum(profile)*stepAngle;
        t = 0:ts:T-ts;
    end
end