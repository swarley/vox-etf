# frozen_string_literal: true

# Container for Vox components.
module Vox
  # Default ETF adapter for vox's gateway component.
  module ETF
    # @!parse [ruby]
    #   # Encode an object to an ETF term. This method accepts, `Integer`, `Float`,
    #   # `String`, `Symbol`, `Hash`, `Array`, `nil`, `true`, and `false` objects.
    #   # It also allows any object that responds to `#to_hash => Hash`. 
    #   # @param input [Object, #to_hash] The object to be encoded as an ETF term.
    #   # @return [String] The ETF term encoded as a packed string.
    #   def self.encode(input)
    #   end
    
    # @!parse [ruby]
    #   # Decode an ETF term from a string.
    #   # @param input [String] The ETF term to be decoded.
    #   # @return [Object] The ETF term decoded to an object.
    #   def self.decode(input)
    #   end

    # Gem version
    VERSION = '0.1.4'
  end
end
