# Vox::ETF

## Installation

Add this line to your application's Gemfile:

```ruby
gem 'vox-etf'
```

And then execute:

    $ bundle install

Or install it yourself as:

    $ gem install vox-etf

## Usage

To use the encoder/decoder independently use the respective `#encode` and `#decode` methods.

```ruby
    require 'vox/etf'

    p Vox::ETF.decode("\x83l\x00\x00\x00\x03a\x01a\x02a\x03j")
    # => [1, 2, 3]

    p Vox::ETF.encode({foo: "bar"})
    # => "\x83t\x00\x00\x00\x01w\x03foom\x00\x00\x00\x03bar"
```


To use with the Vox gateway, add this gem to your Gemfile and provide `:etf` as the encoding option to `Vox::Gateway::Client#initialize`.

## Contributing

Bug reports and pull requests are welcome on GitHub at https://github.com/swarley/vox-etf. This project is intended to be a safe, welcoming space for collaboration, and contributors are expected to adhere to the [code of conduct](https://github.com/swarley/vox-etf/blob/master/CODE_OF_CONDUCT.md).


## License

The gem is available as open source under the terms of the [MIT License](https://opensource.org/licenses/MIT).

## Code of Conduct

Everyone interacting in the Vox::Etf project's codebases, issue trackers, chat rooms and mailing lists is expected to follow the [code of conduct](https://github.com/[USERNAME]/vox-etf/blob/master/CODE_OF_CONDUCT.md).
