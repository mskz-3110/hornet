require "buildrake"

desc "Setup"
task :setup do
  Buildrake.setup
end

desc "Build <platform> <configuration>"
task :build do
  ARGV.shift
  platform = ARGV.shift
  configuration = ARGV.shift
  if platform.nil? || configuration.nil?
    yaml = YAML.load_file( "#{Buildrake::Rush.full_dir_path( __FILE__ )}/buildrake.yaml" )
    yaml[ "anchors" ][ "platforms" ].each{|platform, _|
      [ "debug", "release" ].each{|configuration|
        case platform
        when "android"
          android_ndk = "/tmp/android-ndk-r10e"
          puts "BUILDRAKE_ANDROID_NDK=#{android_ndk} rake build #{platform} #{configuration}"
          puts "BUILDRAKE_ANDROID_NDK=#{android_ndk} BUILDRAKE_ANDROID_STL=c++_static rake build #{platform} #{configuration}"
          puts "BUILDRAKE_ANDROID_NDK=#{android_ndk} BUILDRAKE_ANDROID_STL=gnustl_static rake build #{platform} #{configuration}"
        when "windows"
          # TODO
        else
          puts "rake build #{platform} #{configuration}"
        end
      }
    }
  else
    Buildrake::Rush.changed( "build/#{platform}" ){
      Buildrake::Rush.sh( "BUILDRAKE_CONFIGURATION=#{configuration} ruby ./build.rb" )
    }
  end
  exit 0
end

Buildrake::Rush.find( "*.rake" ){|path|
  load Buildrake::Rush.full_file_path( path )
}
