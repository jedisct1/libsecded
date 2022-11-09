const std = @import("std");
const LibExeObjStep = std.build.LibExeObjStep;

pub fn build(b: *std.build.Builder) void {
    // Standard release options allow the person running `zig build` to select
    // between Debug, ReleaseSafe, ReleaseFast, and ReleaseSmall.
    const mode = b.standardReleaseOptions();
    var target = b.standardTargetOptions(.{});

    const static = b.addStaticLibrary("secded", null);
    const shared = b.addSharedLibrary("secded", null, .{ .versioned = .{ .major = 1, .minor = 0 } });
    shared.strip = true;
    static.strip = true;

    const libs = [_]*LibExeObjStep{ static, shared };
    for (libs) |lib| {
        lib.setTarget(target);
        lib.setBuildMode(mode);
        lib.install();
        lib.linkLibC();
        lib.addIncludePath(".");
        lib.addCSourceFiles(&.{
            "expand.c", "parity.c", "fill.c", "correct.c", "shrink.c", "encode.c", "clean.c", "decode.c",
        }, &.{});
    }

    const exe = b.addExecutable("test", null);
    exe.setTarget(target);
    exe.setBuildMode(mode);
    exe.linkLibrary(static);
    exe.addCSourceFile("test.c", &.{});

    const test_cmd = exe.run();
    const test_step = b.step("test", "Run the test app");
    test_step.dependOn(&test_cmd.step);
}
