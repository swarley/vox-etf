# frozen_string_literal: true

RSpec.describe Vox::ETF do
  it 'has a version number' do
    expect(Vox::ETF::VERSION).not_to be nil
  end

  describe '.decode' do
    context 'when the term data is invalid' do
      let(:bad_version_data) { [130].pack('C*') }
      let(:bad_term_id) { [131, 200].pack('C*') }
      let(:oob_data) { [131, 98, 1].pack('C*') }

      it 'raises an exception for an invalid ETF version' do
        expect { described_class.decode(bad_version_data) }.to raise_error(ArgumentError)
      end

      it 'raises an exception for an invalid term ID' do
        expect { described_class.decode(bad_term_id) }.to raise_error(ArgumentError)
      end

      it 'raises an exception when there are insufficient bytes' do
        expect { described_class.decode(oob_data) }.to raise_error(RangeError)
      end
    end

    context 'when the term is SMALL_INTEGER_EXT' do
      let(:small_int) { 21 }
      let(:small_int_data) { [131, 97, small_int].pack('C*') }

      it 'decodes SMALL_INTEGER_EXT' do
        expect(described_class.decode(small_int_data)).to eq small_int
      end
    end

    context 'when the term is INTEGER_EXT' do
      let(:int) { 10 }
      let(:int_data) { [131, 98, int].pack('CCl>') }

      it 'decodes to an integer' do
        expect(described_class.decode(int_data)).to eq int
      end
    end

    it 'decodes FLOAT_EXT' do
      skip 'Not implemented'
    end

    context 'when the term is SMALL_TUPLE_EXT' do
      let(:small_tuple) { [1, 2] }
      let(:small_tuple_data) { [131, 104, 2, 97, 1, 97, 2].pack('C*') }

      it 'decodes to an array' do
        expect(described_class.decode(small_tuple_data)).to eq small_tuple
      end
    end

    context 'when the term is LARGE_TUPLE_EXT' do
      let(:large_tuple) { [1, 2] }
      let(:large_tuple_data) { [131, 105, 2, 97, 1, 97, 2].pack('CCl>C*') }

      it 'decodes to an array' do
        expect(described_class.decode(large_tuple_data)).to eq large_tuple
      end
    end

    context 'when the term is MAP_EXT' do
      let(:map) { { 1 => 2 } }
      let(:map_data) { [131, 116, 1, 97, 1, 97, 2].pack('CCl>C*') }

      it 'decodes to a hash' do
        expect(described_class.decode(map_data)).to eq map
      end
    end

    context 'when the term is NIL_EXT' do
      let(:nil_data) { [131, 106].pack('C*') }

      it 'decodes to an empty array' do
        expect(described_class.decode(nil_data)).to eq []
      end
    end

    context 'when the term is STRING_EXT' do
      let(:string) { 'foo' }
      let(:string_data) { [131, 107, string.size, *string.bytes].pack('CCS>C*') }

      it 'decodes to a byte array' do
        expect(described_class.decode(string_data)).to eq string.chars.map(&:ord)
      end
    end

    context 'when the term is LIST_EXT' do
      let(:list) { [1, 2] }
      let(:list_data) { [131, 108, 2, 97, 1, 97, 2, 106].pack('CCl>C*') }
      let(:improper_list_data) { [131, 108, 2, 97, 1, 97, 2, 97, 3].pack('CCl>C*') }

      it 'decodes to an array' do
        expect(described_class.decode(list_data)).to eq list
      end

      it 'raises an error for improper lists' do
        expect { described_class.decode(improper_list_data) }.to raise_error(ArgumentError)
      end
    end

    context 'when the term is BINARY_EXT' do
      let(:binary) { 'foo' }
      let(:binary_data) { [131, 109, binary.length, *binary.bytes].pack('CCl>C*') }

      it 'decodes to a string' do
        expect(described_class.decode(binary_data)).to eq binary
      end
    end

    context 'when the term is NEW_FLOAT_EXT' do
      let(:float) { 1.23456789 }
      let(:float_data) { [131, 70, float].pack('CCG') }

      it 'decodes to a float' do
        expect(described_class.decode(float_data)).to eq float
      end
    end

    context 'when the term is ATOM_UTF8_EXT or ATOM_EXT' do
      let(:atom) { :foo }
      let(:atom_data) { [131, 118, atom.length, *atom.to_s.bytes].pack('CCS>C*') }

      it 'decodes to a symbol' do
        expect(described_class.decode(atom_data)).to eq atom
      end
    end

    context 'when the term is SMALL_ATOM_UTF8_ETX or SMALL_ATOM' do
      let(:atom) { :foo }
      let(:atom_data) { [131, 119, atom.length, *atom.to_s.bytes].pack('C*') }

      it 'decodes to a symbol' do
        expect(described_class.decode(atom_data)).to eq atom
      end
    end

    context 'when the term is SMALL_BIG_EXT' do
      let(:bignum) { 123_456_789_123_456_789 }
      let(:bignum_data) { [131, 110, 8, 0, 21, 95, 208, 172, 75, 155, 182, 1].pack('C*') }

      it 'decodes to an integer' do
        expect(described_class.decode(bignum_data)).to eq bignum
      end
    end

    context 'when the term is LARGE_BIG_EXT' do
      let(:bignum) { 123_456_789_123_456_789 }
      let(:bignum_data) { [131, 111, 8, 0, 21, 95, 208, 172, 75, 155, 182, 1].pack('CCl>C*') }

      it 'decodes to an integer' do
        expect(described_class.decode(bignum_data)).to eq bignum
      end
    end
  end
end
