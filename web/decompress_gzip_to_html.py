import gzip
import io
import sys
from pathlib import Path

#!! NEEDED TO BE UPDATED AND REFACTORED, REWRITE

def decompress_gz_bytes(gz_bytes: bytes) -> str:
    """
    Decompress a GZIP-formatted bytes object back into an HTML string
    """
    # Create a binary stream from the GZIP file
    compressed_stream = io.BytesIO(gz_bytes)
    
    # Read compressed data from the bytes stream
    with gzip.GzipFile(mode='rb', fileobj=compressed_stream) as f:
        decompressed_bytes = f.read()
    
    # Return content as string
    return decompressed_bytes.decode('utf-8')

def main(args):
    gzip_file_path = Path(args[0])
    
    try:
        with open(gzip_file_path, mode='rb') as f:
            gzip_file_path_bytes = f.read()
        
        # Decompress content
        html_string = decompress_gz_bytes(gzip_file_path_bytes)
        
        # Create an HTML file out of the string
        html_path = gzip_file_path.with_suffix("") # example.html.gz -> example.html
        
        with open(html_path, mode='w') as f:
            f.write(html_string)
            
        print(f"\nSuccessfully decompressed \"{gzip_file_path}\" to \"{html_path}\"")
            
    except FileNotFoundError:
        print(f"\nThe file \"{gzip_file_path}\" was not found.")
    except Exception as e:
        print(f"An error has occured during processing: {e}")
    
if __name__ == "__main__":
    main(sys.argv[1:])