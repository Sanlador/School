using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class controller : MonoBehaviour
{
    public GameObject arrowObject;

    int dataIndex = 0;
    bool mode = false;
    List<float[]> data;
    List<GameObject> arrows;

    // Start is called before the first frame update
    void Start()
    {
        data = readData("Assets//Data//data.csv", dataIndex);
        arrows = displayDataWave(data);
    }

    // Update is called once per frame
    void Update()
    {
        if (Input.GetButton("Horozontal") && Input.GetAxisRaw("Horizontal") > 0)
        {
            foreach (GameObject arrow in arrows)
            {
                arrow.transform.Translate(new Vector3(2F, 0, 0) * Time.deltaTime);
            }
        }
        else if (Input.GetButton("Horozontal") && Input.GetAxisRaw("Horizontal") < 0)
        {
            foreach (GameObject arrow in arrows)
            {
                arrow.transform.Translate(new Vector3(-2F, 0, 0) * Time.deltaTime);
            }
        }

        if (Input.GetButton("Vertical") && Input.GetAxisRaw("Vertical") > 0)
        {
            if (dataIndex < 9 && !mode)
            {
                foreach (GameObject arrow in arrows)
                    Destroy(arrow);
                dataIndex++;
                data = readData("Assets//Data//data.csv", dataIndex);
                arrows = displayDataGrid();
            }
        }
        else if (Input.GetButton("Vertical") && Input.GetAxisRaw("Vertical") < 0)
        {
            if (dataIndex > 0 !mode)
            {
                foreach (GameObject arrow in arrows)
                    Destroy(arrow);
                dataIndex--;
                data = readData("Assets//Data//data.csv", dataIndex);
                arrows = displayDataGrid();
            }
        }

        if (Input.GetButton("Mode"))
        {
            mode = !mode;

            if (mode)
            {
                foreach (GameObject arrow in arrows)
                    Destroy(arrow);
                arrows = displayDataGrid();
            }
            else
            {
                foreach (GameObject arrow in arrows)
                    Destroy(arrow);
                data = readData("Assets//Data//data.csv", dataIndex);
                arrows = displayDataWave(data);
            }
        }
    }

    List<GameObject> displayDataWave(List<float[]> data)
    {
        List<GameObject> arrows = new List<GameObject>();
        float gdp;
        for (int i = 0; i < 100; i++)
        {
            gdp = 0F;
            for (int j = 0; j < 13; j++)
            {
                //get data from table
                float growth = data[i][j];
                gdp += growth;

                GameObject arrow = Object.Instantiate(arrowObject);
                arrow.GetComponent<Arrow>().wave(growth, gdp, new Vector3(.5F * i, 0F, .5F * j));
                arrows.Add(arrow);
            }
            
        }
        return arrows;
    }

    List<GameObject> displayDataGrid()
    {
        List<GameObject> arrows = new List<GameObject>();
        for (int k = 0; k < 100; k++)
        {
            List<float[]> data = readData("Assets//Data//data.csv", k);
            float gdp;
            for (int i = 0; i < 100; i++)
            {
                gdp = 0F;
                for (int j = 0; j < 1; j++)
                {
                    //get data from table
                    float growth = data[i][j];
                    gdp += growth;

                    GameObject arrow = Object.Instantiate(arrowObject);
                    arrow.GetComponent<Arrow>().grid(growth, gdp, new Vector3(.5F * k, 0F, .5F * i));
                    arrows.Add(arrow);
                }

            }
            
        }
        return arrows;
    }

    List<float[]> readData(string location, int offset)
    {
        string[] table = System.IO.File.ReadAllLines(location);
        string[] growthValues;
        List<float[]> growth = new List<float[]>();

        for (int i = 1; i <= 100; i++)
        {
            growth.Add(new float[13]);
            growthValues = table[i + 100 * offset].Split(',');

            for (int j = 0; j < 13; j++)
            {
                growth[i - 1][j] = float.Parse(growthValues[3 * j + 7]);
            }
        }

        return growth;
    }
}
