# Epaperify

A gem for interfacing with a waveshare E-ink panel

## Installation

There are a bunch of things you need to do to make this work:

Raspberry pi 4
--------------

Enable SPI

```
sudo raspi-config
```

Under Interfacing Options -> Enable SPI

Then reboot the pi

Install the bcm2835 libraries

```
wget http://www.airspayce.com/mikem/bcm2835/bcm2835-1.60.tar.gz
tar zxvf bcm2835-1.60.tar.gz 
cd bcm2835-1.60/
sudo ./configure
sudo make
sudo make check
sudo make install
```

Install Ruby Dev libraries

```
sudo apt install ruby-dev
```

Add this line to your application's Gemfile:

```ruby
gem 'epaperify'
```

And then execute:

    $ bundle install

Or install it yourself as:

    $ gem install epaperify

## Usage

Create a canvas passing the E-ink model as well as a rotation

```ruby
require "epaperify"

canvas = Epaperify::PaperCanvas.new(Epaperify::DISPLAY_EPD_5IN83_V2, 90)
```

There are various things you can do on the canvas.

```ruby
canvas.font_size = 24
bitmap = Epaperify::ImageBuffer.new("sample/sample.bmp")
canvas.draw_bitmap(bitmap, 0, 0)
puts canvas.measure "Hello World!"
canvas.print "Hello World!"
```

Show them on the screen

```ruby
canvas.show
```

Note: You need superuser to access /dev/gpiomem so ruby scripts have to be run under sudo

## Development

After checking out the repo, run `bin/setup` to install dependencies. Then, run `rake spec` to run the tests. You can also run `bin/console` for an interactive prompt that will allow you to experiment.

To install this gem onto your local machine, run `bundle exec rake install`. To release a new version, update the version number in `version.rb`, and then run `bundle exec rake release`, which will create a git tag for the version, push git commits and the created tag, and push the `.gem` file to [rubygems.org](https://rubygems.org).

## Contributing

Bug reports and pull requests are welcome on GitHub at https://github.com/[USERNAME]/epaperify. This project is intended to be a safe, welcoming space for collaboration, and contributors are expected to adhere to the [code of conduct](https://github.com/[USERNAME]/epaperify/blob/master/CODE_OF_CONDUCT.md).

## License

The gem is available as open source under the terms of the [MIT License](https://opensource.org/licenses/MIT).

## Code of Conduct

Everyone interacting in the Epaperify project's codebases, issue trackers, chat rooms and mailing lists is expected to follow the [code of conduct](https://github.com/[USERNAME]/epaperify/blob/master/CODE_OF_CONDUCT.md).
