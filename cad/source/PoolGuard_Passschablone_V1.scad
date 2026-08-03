// PoolGuard Passschablone V1
// Fit-test frame for skimmer compartments 3 and 4.
// This is not the final housing.

$fn = 180;

lid_diameter = 242.0;
compartment_depth = 78.5;
radial_clearance = 0.5;
angular_clearance = 0.6;
wall = 2.0;
height = 5.0;

outer_radius = lid_diameter / 2 - radial_clearance;
inner_radius = lid_diameter / 2 - compartment_depth + radial_clearance;
start_angle = -30 + angular_clearance;
end_angle = 30 - angular_clearance;

module sector_2d(ri, ro, a0, a1, step = 0.5) {
    polygon(points = concat(
        [for (a = [a0 : step : a1]) [ro * cos(a), ro * sin(a)]],
        [for (a = [a1 : -step : a0]) [ri * cos(a), ri * sin(a)]]
    ));
}

linear_extrude(height = height)
difference() {
    sector_2d(inner_radius, outer_radius, start_angle, end_angle);
    sector_2d(
        inner_radius + wall,
        outer_radius - wall,
        start_angle + 0.8,
        end_angle - 0.8
    );
}
