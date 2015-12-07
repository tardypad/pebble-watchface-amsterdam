import os.path
import waflib.Logs
from glob import glob
from sh import Command
from waflib.Build import POST_LAZY

top = '.'
out = 'build'

def options(ctx):
    ctx.load('pebble_sdk')

def configure(ctx):
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
    try:
      rsvgconvert = Command('rsvg-convert')
      apngasm = Command('apngasm')
    except:
      waflib.Logs.warn('Missing required sh commands: rsvg-convert, apngasm')
      waflib.Logs.warn("Can't rebuild animations, will use current ones if available")
      return

    def generate_animation(task):
        animation_dir = task.env.ANIMATION_DIR
        for svg_frame in glob(animation_dir + '/frames/*.svg'):
            rsvgconvert(svg_frame, o=svg_frame.replace('.svg', '.png'))
        apngasm('--force', o=animation_dir + '/xxx.apng', f=animation_dir + '/animation.xml')
        for png_frame in glob(animation_dir + '/frames/*.png'):
            os.remove(png_frame)

    for animation_dir in ctx.path.ant_glob('resources/animations/*', dir=True, src=False):
        animation_sources = ctx.path.ant_glob([animation_dir.srcpath()+'/**/*.svg', animation_dir.srcpath()+'/**/*.xml'])
        task_env = ctx.env.derive()
        task_env.ANIMATION_DIR = animation_dir.abspath()
        task_name = 'generate ' + os.path.basename(animation_dir.abspath()) + ' animation'
        ctx(rule=generate_animation, source=animation_sources, env=task_env, name=task_name, color='YELLOW')
