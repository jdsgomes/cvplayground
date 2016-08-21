from scipy.ndimage import *
from pylab import *

# harris corner detector with Noble's corner measure
# see https://en.wikipedia.org/wiki/Corner_detection
def find_corners(im, sigma = 1, min_dist = 15, threshold_factor = 0.1):
    """ detect corners in the input image
    Parameters
    ----------
    im : numpy.ndarray
       Input image.
    sigma : int
        Standard deviation for Gaussian kernel
    min_dist : int
        Mininum distance between detected corners
    threshold_factor : float
        Relative threshold to filter the corner detections.
        Absolute threhold is calculated by multiplying threshold_factor
        with detected value.
    """
    # edges x and y direction
    imx = zeros(im.shape)
    filters.gaussian_filter(im, (sigma,sigma), (0,1), imx)
    imy = zeros(im.shape)
    filters.gaussian_filter(im, (sigma,sigma), (1,0), imy)

    # compute components of the Harris matrix
    Ixx = filters.gaussian_filter(imx*imx,sigma)
    Ixy = filters.gaussian_filter(imx*imy,sigma)
    Iyy = filters.gaussian_filter(imy*imy,sigma)
    
    # determinant
    Adet = Ixx*Iyy - Ixy**2

    # trace - not needed in the current version see bellow when calculating Mc
    # Atr = Ixx + Iyy
 
    Mc = Adet # works better than the Mc/Wtr + 0.0001

    # find top corner candidates above a threshold_factor
    corner_threshold = Mc.max() * threshold_factor
    candidates = (Mc > corner_threshold) * 1

    # get coordinates of candidates
    coords = array(candidates.nonzero()).T

    # get values for the candidates
    candidates_values = [Mc[c[0],c[1]] for c in coords]

    # sort candidates
    index = argsort(candidate_values)

    # store allowed point locations in array
    # initially all locations are allowed
    allowed_locations = ones(Mc.shape)
    allowed_locations[min_dist:-min_dist,min_dist:-min_dist] = 1

    # select points one by one based on the highest scores and 
    # for each one supress the region around it
    filtered_coords = []
    for i in index:
        if allowed_locations[coords[i,0],coords[i,1]] == 1:
            filtered_coords.append(coords[i])
            allowed_locations[max(0,(coords[i,0]-min_dist)):min(im.shape[0]-1,(coords[i,0]+min_dist)),
            max(0,(coords[i,1]-min_dist)):min(im.shape[1]-1,(coords[i,1]+min_dist))] = 0
    return filtered_coords