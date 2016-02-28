import os.path
import waflib.Logs
from glob import glob
from waflib.Build import POST_LAZY

top = '.'
out = 'build'

def options(ctx):
    ctx.load('pebble_sdk')

def configure(ctx):
    try:
      ctx.find_program('convert', var='CONVERT')
      ctx.find_program('gif2apng', var='GIF2APNG')
    except ctx.errors.ConfigurationError:
      waflib.Logs.warn('Missing required programs: convert, gif2apng')
      waflib.Logs.warn("Can't rebuild animations, will use current ones if available")

    ctx.load('pebble_sdk')

def build(ctx):
    ctx.post_mode = POST_LAZY

    ctx.add_group('animations')
    generate_animations(ctx)

    ctx.load('pebble_sdk')

    build_worker = os.path.exists('worker_src')
    binaries = []

    for p in ctx.env.TARGET_PLATFORMS:
        ctx.set_env(ctx.all_envs[p])
        ctx.set_group(ctx.env.PLATFORM_NAME)
        app_elf='{}/pebble-app.elf'.format(ctx.env.BUILD_DIR)
        ctx.pbl_program(source=ctx.path.ant_glob('src/**/*.c'),
        target=app_elf)

        if build_worker:
            worker_elf='{}/pebble-worker.elf'.format(ctx.env.BUILD_DIR)
            binaries.append({'platform': p, 'app_elf': app_elf, 'worker_elf': worker_elf})
            ctx.pbl_worker(source=ctx.path.ant_glob('worker_src/**/*.c'),
            target=worker_elf)
        else:
            binaries.append({'platform': p, 'app_elf': app_elf})

    ctx.set_group('bundle')
    ctx.pbl_bundle(binaries=binaries, js=ctx.path.ant_glob('src/js/**/*.js'))

def generate_animations(ctx):
    if not ctx.env.CONVERT or not ctx.env.GIF2APNG:
      return

    for platform in ctx.env.TARGET_PLATFORMS:
        for animation_dir in ctx.path.ant_glob('resources/animations/*', dir=True, src=False):
            animation_dir_platform = animation_dir.srcpath() + '/' + platform
            animation_sources = ctx.path.ant_glob([animation_dir_platform + '/**/*.svg', animation_dir.srcpath() + '/animation.sh'])
            animation_script = animation_dir.abspath() + '/animation.sh ' + str(platform)
            task_name = 'generate ' + os.path.basename(animation_dir.abspath()) + ' animation for ' + platform
            ctx(rule=animation_script, source=animation_sources, name=task_name, color='YELLOW')
