MRuby::Gem::Specification.new('mruby-bin-mrbmacs-gtk') do |spec|
  spec.license = 'MIT'
  spec.author  = 'masahino'
  spec.version = '0.2.0'
  spec.add_dependency('mruby-scintilla-gtk')
  spec.add_dependency('mruby-iconv')
  spec.add_dependency('mruby-mrbmacs-base')
  spec.bins = %w(mrbmacs-gtk)
end
