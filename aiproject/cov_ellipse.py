# modified code based on  https://github.com/joferkington/oost_paper_code/blob/master/error_ellipse.py
# found in post on SO: http://stackoverflow.com/questions/12301071/multidimensional-confidence-intervals
import numpy as np
import matplotlib.pyplot as plt
from matplotlib.patches import Ellipse

def plot_cov_ellipse(cov, pos, ax=None, **kwargs):
    """
    Plots a 1 and 2 sigma error ellipses based on the specified covariance
    matrix (`cov`). Additional keyword arguments are passed on to the 
    ellipse patch artist.

    Parameters
    ----------
        cov : The 2x2 covariance matrix to base the ellipse on
        pos : The location of the center of the ellipse. Expects a 2-element
            sequence of [x0, y0].
        ax : The axis that the ellipse will be plotted on. Defaults to the 
            current axis.
        Additional keyword arguments are pass on to the ellipse patch.

    Returns
    -------
        A matplotlib ellipse artist
    """
    def eigsorted(cov):
        vals, vecs = np.linalg.eigh(cov)
        order = vals.argsort()[::-1]
        return vals[order], vecs[:,order]

    if ax is None:
        ax = plt.gca()

    vals, vecs = eigsorted(cov)
    theta = np.degrees(np.arctan2(*vecs[:,0][::-1]))

    # first the inner std = 2 dashed ellipse
    # Width and height are "full" widths, not radius
    width, height = 2 * 2 * np.sqrt(vals)
    ellip1 = Ellipse(xy=pos, width=width, height=height, angle=theta, color='c', fill=False, linestyle='dashed')

    # then the outter std = 10 ellipse
    # Width and height are "full" widths, not radius
    width, height = 2 * 10 * np.sqrt(vals)
    ellip2 = Ellipse(xy=pos, width=width, height=height, angle=theta, color='c', fill=False)

    # Width and height are "full" widths, not radius
    width, height = 2 * 15 * np.sqrt(vals)
    ellip3 = Ellipse(xy=pos, width=width, height=height, angle=theta, color='c', fill=False)

    ax.add_artist(ellip1)
    ax.add_artist(ellip2)
    ax.add_artist(ellip3)
    return ellip2