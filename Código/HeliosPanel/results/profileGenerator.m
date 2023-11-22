%% CONFIGURATION
t_period = 4;
t_end = 4;
peak = 3500;
s = 1;

%% PROFILE GENERATION
[pp, p, t] = sinusoidal(t_period, t_end, peak, s);

steps = [0, round(diff(pp)/s)];
k = find(steps);
t_flank = t(k);

prof.t = [ceil(diff(t_flank)*1e6), 0];
prof.d = steps(k)>=0;

%% PLOTTING
figure; set(gcf, 'Color', 'w');
    ax1=subplot(221); grid on; hold on;
        plot(t, p);
        stairs(t, pp);
        title("Position");
        legend(["Reference", "Stepped"])
        xlabel("time [s]")
        ylabel("steps count")

    ax2=subplot(222); grid on; hold on;
        stairs(t_flank, prof.t);
        title("Profile: step time");
        xlabel("time [s]")
        ylabel("step time [us]")
        
    ax3=subplot(223); grid on; hold on;
        stairs(t, steps);
        title("Steps");
        xlabel("time [s]")
        ylabel("step")

    ax4=subplot(224); grid on; hold on;
        stairs(t_flank, prof.d);
        title("Profile: step direction");
        xlabel("time [s]")
        ylabel("direction (0/1)")
linkaxes([ax1, ax2, ax3, ax4], 'x');

%% FUNCTIONS
function [pp, p, t] = sinusoidal(t_period, t_end, peak, s)
    N = 1e6;
    f = @(t) peak*sin(2*pi*t/t_period);
    t = 0:t_end/N:t_end;
    p = f(t);
    pp = round(p/s)*s;
end