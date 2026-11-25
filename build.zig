const std = @import("std");

pub fn build(b: *std.Build) void {
    const target = b.standardTargetOptions(.{});
    const optimize = b.standardOptimizeOption(.{});

    const module = b.createModule(.{
        .target = target,
        .optimize = optimize,
        .link_libc = true,
        .strip = true,
    });
    module.addIncludePath(b.path("."));
    module.addCSourceFiles(.{ .files = &.{
        "expand.c", "parity.c", "fill.c", "correct.c", "shrink.c", "encode.c", "clean.c", "decode.c",
    } });
    const static = b.addLibrary(.{
        .name = "secded",
        .linkage = .static,
        .root_module = module,
    });
    const shared = b.addLibrary(.{
        .name = "secded",
        .linkage = .dynamic,
        .version = .{ .major = 1, .minor = 0, .patch = 0 },
        .root_module = module,
    });
    b.installArtifact(static);
    b.installArtifact(shared);

    const test_module = b.createModule(.{
        .target = target,
        .optimize = optimize,
    });
    test_module.linkLibrary(static);
    test_module.addCSourceFile(.{ .file = b.path("test.c") });
    const exe = b.addExecutable(.{ .name = "test", .root_module = test_module });
    const test_cmd = b.addRunArtifact(exe);
    const test_step = b.step("test", "Run the test app");
    test_step.dependOn(&test_cmd.step);
}
