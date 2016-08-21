from scipy.ndimage import *
from PIL import *
from find_corners import *

if len(sys.argv) > 1:
    input_image = sys.argv[1]
    try:
        # load input image
        im = array(Image.open(input_image).convert('L')) 

        # detect corners in the input image
        corners_positions = find_corners(im)

        n_corners = len(corners_positions)

        # classify shape according to the number of corners detected
        if n_corners == 3:
            print 'Triangle'
        elif n_corners == 4:
            print 'Rectangle'
        elif n_corners == 10:
            print 'Star';
        elif n_corners > 10:
            print 'Circle';
        else:
            print 'unkown shape';

    except IOError:
        print 'Unable to load input image: ' , input_image
        print 'Exception: ', IOError

else:
    print 'Usage ', sys.argv[0], ' input_image'