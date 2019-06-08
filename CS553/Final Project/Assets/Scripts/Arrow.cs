using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class Arrow : MonoBehaviour
{
    Vector3 coord;
    Vector3 scale;
    Color color;
    float r, g, b;

    //adjust as needed for display purposes
    const float scaleCoef = .01F;
    const float coordCoef = 1F;
    const float growthCoef = 100F;
    const float rotCoef = .2F;
    const float gdpCoef = 10F;
    const float colorCoef = .25F;
    float rot;

    public GameObject arrow;

    public Color colorTransform(float growth)
    {
        //set color based on growth
        if (growth > 1.65)
        {
            r = 1F;
            if (growth > 3.3)
            {
                g = 1F;
                if (growth >= 4.5F)
                    b = 1F;
                else
                    b = growth * colorCoef - .66F;
            }
            else
            {
                g = growth * colorCoef - .33F;
                b = 0F;
            }

        }
        else
        {
            r = growth;
            g = 0F;
            b = 0F;
        }
        return new Color(r, g, b); 
    }

    public void wave(float growth, float gdp, Vector3 position)
    {
        growth *= growthCoef;
        
        rot = -90F * growth * rotCoef;
        if (rot < -90F)
            rot = -90F;

        color = colorTransform(growth);
        Renderer rend = GetComponent<Renderer>();
        rend.material.shader = Shader.Find("_Color");
        rend.material.SetColor("_Color", color);
        rend.material.shader = Shader.Find("Specular");
        rend.material.SetColor("_SpecColor", color);

        arrow.transform.localScale += new Vector3(.1F * scaleCoef * growth, .1F * scaleCoef * growth, scaleCoef * growth);
        arrow.transform.Translate(position + new Vector3(0F, gdp * gdpCoef, 0F));
        arrow.transform.Rotate(rot, 0F, 0F);
    }

    public void grid(float growth, float gdp, Vector3 position)
    {
        growth *= growthCoef;

        color = colorTransform(growth);
        Renderer rend = GetComponent<Renderer>();
        rend.material.shader = Shader.Find("_Color");
        rend.material.SetColor("_Color", color);
        rend.material.shader = Shader.Find("Specular");
        rend.material.SetColor("_SpecColor", color);

        arrow.transform.Translate(position);
        arrow.transform.Rotate(-90F, 0F, 0F);
    }
}
