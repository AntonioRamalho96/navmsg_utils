"""
WordSerializer Module

This module provides an interface to serialize RinexNavRecord protobuf messages into binary strings based on a CSV configuration.
Usage:
    >>> import word_serializer
    >>> config = word_serializer.WordSerializer.get_word_config("config.csv")
    >>> serializer = word_serializer.WordSerializer(config)
    >>> binary_string = serializer.serialize(serialized_record)
"""

class WordSerializer:
    def __init__(self, csv_path : str):
        """
        Initializes the WordSerializer with the path to a CSV configuration file.

        Args:
            csv_path (str): The file path to the CSV configuration.
        """
        pass

    def serialize(self, record : str, wn : int, tow : int ) -> str:
        """
        Serializes a RinexNavRecord protobuf message into a binary string based on the configuration.

        Args:
            record (str): The RinexNavRecord protobuf message to serialize (serialized using SerializeToString()).
            wn (int): Week number
            tow (int): Time of Week (seconds)

        Returns:
            str: The serialized binary string.
        """
        pass

class GalileoEncoder:
    @classmethod
    def encode(cls, input : str) -> str:
        """
        Encodes a string of ones and zeros using the galileo convolutional encoder

        Args:
            input (str): The input is the data to be encoded (including tail bits) as 
                         a string of ones and zeros (e.g. "100101001110")
        Returns:
            str: The encoded bits, also as a string of ones and zeros
        """
        pass

    @classmethod
    def interleave_inav(cls, input : str) -> str:
        """
        Interleave INAV bits using an 8-row by 30-column block interleaver.

        Args:
            input (str): String of '0'/'1' characters. Length should be 240
                         (or the caller handles padding).

        Returns:
            str: Interleaved bit string, same length as input.
        """
        pass

    @classmethod
    def interleave_fnav(cls, input : str) -> str:
        """
        Interleave FNAV bits using an 8-row by 61-column block interleaver.

        Args:
            input (str): String of '0'/'1' characters. Length should be 488
                         (or the caller handles padding).

        Returns:
            str: Interleaved bit string, same length as input.
        """
        pass

    @classmethod
    def deinterleave_inav(cls, input : str) -> str:
        """
        Deinterleave INAV bits that were interleaved with the 8x30 block interleaver.

        Args:
            input (str): Interleaved bit string (string of '0'/'1') of length 240.

        Returns:
            str: Deinterleaved bit string, same length as input.
        """
        pass

    @classmethod
    def deinterleave_fnav(cls, input : str) -> str:
        """
        Deinterleave FNAV bits that were interleaved with the 8x61 block interleaver.

        Args:
            input (str): Interleaved bit string (series of '0'/'1') of length 488.

        Returns:
            str: Deinterleaved bit string, same length as input.
        """
        pass


class Crc:
    @classmethod
    def crc24(cls, inout : str) -> str:
        """
        Computes a 24 bits CRC as per the galileo ICD

        Args:
            input (str): Protected data (string of '0'/'1').

        Returns:
            str: 24 bit crc (string of '0'/'1')
        """
        pass